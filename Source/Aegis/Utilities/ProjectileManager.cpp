// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileManager.h"

#include "Aegis/AegisGameStateBase.h"
#include "Aegis/Enemies/Enemy.h"
#include "Aegis/Structures/NexusBuilding/NexusBuilding.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"

AProjectileManager::AProjectileManager()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRootComponent = CreateDefaultSubobject<USceneComponent>("Scene Root Component");
	SetRootComponent(SceneRootComponent);
}

void AProjectileManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	for (TTuple<UStaticMeshComponent*, FProjectilePackage> Element : ActiveProjectiles)
	{
		UStaticMeshComponent* ProjectileMeshComponent = Element.Key;
		FProjectilePackage* ProjectileData = &Element.Value;
		const float DistanceToTravel = DeltaSeconds * ProjectileData->Speed * 100;

		if (ProjectileData->TargetEnemy)
		{
			FVector ForwardVector = ProjectileData->TargetEnemy->GetActorLocation() - ProjectileMeshComponent->GetComponentLocation();
			ForwardVector.Z = 0;
			ForwardVector.Normalize();
			ProjectileData->ForwardVector = ForwardVector;
			
			// ProjectileMeshComponent->SetWorldRotation((ProjectileData->TargetEnemy->GetActorLocation()-ProjectileMeshComponent->GetComponentLocation()).Rotation());
		}

		FVector TargetLoc = ProjectileMeshComponent->GetComponentLocation() + (ProjectileData->ForwardVector * DistanceToTravel);
		TargetLoc.Z = 200.f;

		ProjectileMeshComponent->SetWorldLocation(TargetLoc, false);

		// todo set lifetime and check every second and push to ProjectilesToRelease on that separate thread
		if (Element.Key->GetComponentLocation().Z < -100 || Element.Key->GetComponentLocation().Z > 1000)
		{
			ProjectilesToRelease.Push(Element.Key);
		}
	}

	while (ProjectilesToRelease.Num() > 0)
	{
		ReleaseProjectile(ProjectilesToRelease.Pop());
	}
}

void AProjectileManager::BeginPlay()
{
	Super::BeginPlay();
}

void AProjectileManager::OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UStaticMeshComponent* ProjectileMesh = Cast<UStaticMeshComponent>(OverlappedComponent);
	AEnemy* Enemy = Cast<AEnemy>(OtherActor);
	if (!ProjectileMesh || !Enemy) { return; }

	FProjectilePackage* ProjectilePackage = &ActiveProjectiles[ProjectileMesh];
	if (ProjectilePackage->DamagePackage.ExplosionRadius <= 0)
	{
		UGameplayStatics::ApplyDamage(Enemy, ProjectilePackage->DamagePackage.PhysicalDamage, GetWorld()->GetFirstPlayerController(), this, UDamageType::StaticClass());
	} else
	{
		DrawDebugSphere(GetWorld(), OverlappedComponent->GetComponentLocation(), ProjectilePackage->DamagePackage.ExplosionRadius, 12, FColor::Red, false, -1, 0, 1);
		const AAegisGameStateBase* GameState = Cast<AAegisGameStateBase>(GetWorld()->GetGameState());
		TArray<AActor*> IgnoredActors;
		IgnoredActors.Add(GameState->AegisMap->NexusBuilding);
		
		UGameplayStatics::ApplyRadialDamage(GetWorld(), ProjectilePackage->DamagePackage.PhysicalDamage, OverlappedComponent->GetComponentLocation(), ProjectilePackage->DamagePackage.ExplosionRadius, UDamageType::StaticClass(), IgnoredActors);
	}
	
	// Mark projectile to be cleaned
	// This must be done at the end of the frame instead of as soon as the overlap event is made,
	// to ensure that it does not break the tick loop
	ProjectilesToRelease.Add(ProjectileMesh);
}

UStaticMeshComponent* AProjectileManager::FireProjectile(const FProjectileDamagePackage DamagePackage, const FVector& Start, const AEnemy* TargetEnemy, const float Speed, UStaticMesh* ProjectileMesh)
{
	const FProjectilePackage ProjectilePackage = FProjectilePackage(DamagePackage, Start, TargetEnemy, Speed);

	// Get a MeshComponent to use from the pool
	UStaticMeshComponent* ProjectileMeshComponent = AcquireProjectile(ProjectilePackage);

	ProjectileMeshComponent->SetWorldLocation(Start);
	ProjectileMeshComponent->SetWorldRotation((TargetEnemy->GetActorLocation()-Start).Rotation());
	
	// Set the projectile mesh
	ProjectileMeshComponent->SetStaticMesh(ProjectileMesh);
	
	ProjectileMeshComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &AProjectileManager::OverlapBegin);
	
	return ProjectileMeshComponent;
	
}

UStaticMeshComponent* AProjectileManager::AcquireProjectile(const FProjectilePackage& ProjectilePackage)
{
	UStaticMeshComponent* Projectile;
	
	if (AvailableProjectiles.Num() != 0)
	{
		Projectile = AvailableProjectiles.Pop();
	} else
	{
		Projectile = Cast<UStaticMeshComponent>(AddComponentByClass(UStaticMeshComponent::StaticClass(), true, GetTransform(), false));
		Projectile->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		Projectile->SetMobility(EComponentMobility::Movable);
		Projectile->SetCollisionResponseToAllChannels(ECR_Ignore);
		Projectile->SetCollisionObjectType(ECC_GameTraceChannel4);
		Projectile->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Overlap);
		Projectile->bHiddenInGame = false;
	}
	
	ActiveProjectiles.Add(Projectile, ProjectilePackage);

	return Projectile;
}

void AProjectileManager::ReleaseProjectile(UStaticMeshComponent* Projectile)
{
	CleanProjectile(Projectile);
	AvailableProjectiles.Add(Projectile);
	ActiveProjectiles.Remove(Projectile);
}

void AProjectileManager::CleanProjectile(UStaticMeshComponent* Projectile)
{
	Projectile->SetStaticMesh(nullptr);
	Projectile->OnComponentBeginOverlap.RemoveAll(this);
	Projectile->SetRelativeLocation(FVector::ZeroVector);
}
