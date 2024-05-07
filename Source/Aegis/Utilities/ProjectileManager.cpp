// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileManager.h"

#include "Aegis/Enemies/Enemy.h"
#include "Chaos/GeometryParticlesfwd.h"
#include "Components/CapsuleComponent.h"
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
		//UE_LOG(LogTemp, Warning, TEXT("Speed: %f"), Element.Value.Speed)
		const float DistanceToTravel = DeltaSeconds * Element.Value.Speed * 100;
		FVector ForwardVector = Element.Value.EndPoint - Element.Value.StartPoint;
		ForwardVector.Normalize();

		const FVector TargetLoc = Element.Key->GetComponentLocation() + (ForwardVector * DistanceToTravel);

		Element.Key->SetWorldLocation(TargetLoc, false);
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
	
	UGameplayStatics::ApplyDamage(Enemy, 10, GetWorld()->GetFirstPlayerController(), this, UDamageType::StaticClass());

	// Mark projectile to be cleaned
	// This must be done at the end of the frame instead of as soon as the overlap event is made,
	// to ensure that it does not break the tick loop
	ProjectilesToRelease.Add(ProjectileMesh);
}

UStaticMeshComponent* AProjectileManager::FireProjectile(const FProjectileDamagePackage DamagePackage, const FVector& Start, const FVector& End, const float Speed, UStaticMesh* ProjectileMesh)
{
	const FProjectilePackage ProjectilePackage = FProjectilePackage(DamagePackage, Start, End, Speed);

	// Get a MeshComponent to use from the pool
	UStaticMeshComponent* ProjectileMeshComponent = AcquireProjectile(ProjectilePackage);

	ProjectileMeshComponent->SetWorldLocation(Start);
	ProjectileMeshComponent->SetWorldRotation((End-Start).Rotation());
	
	// Set the projectile mesh
	ProjectileMeshComponent->SetStaticMesh(ProjectileMesh);
	
	// TODO add overlap events
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
