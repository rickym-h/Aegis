// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileManager.h"

#include "NiagaraFunctionLibrary.h"
#include "PaperFlipbookComponent.h"
#include "Aegis/Core/GameStates/AegisGameStateBase.h"
#include "Aegis/Enemies/Enemy.h"
#include "Aegis/Enemies/Damage/MagicDamageType.h"
#include "Aegis/Enemies/Damage/PhysicalDamageType.h"
#include "Aegis/Map/AegisGameMap.h"
#include "Aegis/Structures/Interfaces/ProjectileCallbackInterface.h"
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

	const int32 CurrentWorldSeconds = GetWorld()->GetTimeSeconds();

	// Process movement of homing projectiles
	for (TTuple<UStaticMeshComponent*, FHomingProjectilePackage> Element : ActiveHomingProjectiles)
	{
		UStaticMeshComponent* ProjectileMeshComponent = Element.Key;
		FHomingProjectilePackage* ProjectileData = &Element.Value;
		
		const float DistanceToTravel = DeltaSeconds * ProjectileData->ProjectileSpeed * 100;

		// If the enemy is valid, rotate toward it
		if (IsValid(ProjectileData->TargetEnemy))
		{
			// Get the current rotation of the mesh
			const FRotator CurrentRotation = ProjectileMeshComponent->GetComponentRotation();

			// Calculate the direction vector from the mesh to the target point
			const FVector Direction = (ProjectileData->TargetEnemy->TargetPoint->GetComponentLocation() - ProjectileMeshComponent->GetComponentLocation()).GetSafeNormal();

			// Convert the direction vector into a rotation
			const FRotator TargetRotation = Direction.Rotation();

			// Interpolate between the current rotation and the target rotation
			const FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaSeconds, ProjectileData->ProjectileRotationSpeed);
			
			// Set the new rotation to the mesh
			ProjectileMeshComponent->SetWorldRotation(NewRotation);
		}

		const FVector TargetLocation = ProjectileMeshComponent->GetComponentLocation() + (ProjectileMeshComponent->GetForwardVector() * DistanceToTravel);

		ProjectileMeshComponent->SetWorldLocation(TargetLocation, false);

		// Despawn projectile if the elapsed time has passed or if it is significantly below the world
		if (CurrentWorldSeconds - ProjectileData->StartLaunchTime > 20 || ProjectileMeshComponent->GetComponentLocation().Z < -10)
		{
			ProjectilesToRelease.Push(Element.Key);
		}
	}

	// Process movement of arcing projectiles
	for (TTuple<UStaticMeshComponent*, FArcProjectilePackage> Element : ActiveArcProjectiles)
	{
		UStaticMeshComponent* ProjectileMeshComponent = Element.Key;
		FArcProjectilePackage* ProjectileData = &Element.Value;
		
		const double TimeSinceLaunch = GetWorld()->TimeSeconds - ProjectileData->StartLaunchTime;
		const double T = TimeSinceLaunch / static_cast<double>(ProjectileData->StartToEndTravelSeconds);

		const FVector StartToCentrePoint = FMath::Lerp(ProjectileData->StartControlPoint, ProjectileData->CentreControlPoint, T);
		const FVector CentreToEndPoint = FMath::Lerp(ProjectileData->CentreControlPoint, ProjectileData->EndControlPoint, T);
		const FVector BezierPoint = FMath::Lerp(StartToCentrePoint, CentreToEndPoint, T);

		// Set the new rotation to the mesh
		// Calculate the direction vector from the mesh to the target point
		FVector Direction = (CentreToEndPoint - BezierPoint).GetSafeNormal();
		
		if (T > 1)
		{
			// If T is beyond the target, the CentreToEndPoint will be behind the projectile, so must point backwards
			Direction = -Direction;
		}
		const FRotator TargetRotation = Direction.Rotation();
		ProjectileMeshComponent->SetWorldRotation(TargetRotation);
		
		ProjectileMeshComponent->SetWorldLocation(BezierPoint, false);
		
		// Despawn projectile if the elapsed time has passed or if it is significantly below the world
		if (CurrentWorldSeconds - ProjectileData->StartLaunchTime > 20 || ProjectileMeshComponent->GetComponentLocation().Z < -10)
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

void AProjectileManager::ApplyDamageAndEffects(const UPrimitiveComponent* OverlappedComponent, const FHitResult& HitResult,
	const UStaticMeshComponent* ProjectileMesh, AEnemy* Enemy, const FProjectileDamagePackage* DamagePackage,
	UNiagaraSystem* OnHitParticleSystem, UObject* ResponsibleSource)
{
	// Apply single-target damage if there is no explosion radius  
	if (DamagePackage->ExplosionRadius <= 0)
	{
		const float PhysicalDamageApplied = UGameplayStatics::ApplyDamage(Enemy, DamagePackage->PhysicalDamage, GetWorld()->GetFirstPlayerController(), this, UPhysicalDamageType::StaticClass());
		const float MagicDamageApplied = UGameplayStatics::ApplyDamage(Enemy, DamagePackage->MagicDamage, GetWorld()->GetFirstPlayerController(), this, UMagicDamageType::StaticClass());

		const float TotalDamageApplied = PhysicalDamageApplied + MagicDamageApplied;
		// Apply callback function if relevant
		if (ResponsibleSource->Implements<UProjectileCallbackInterface>())
		{
			IProjectileCallbackInterface::Execute_SingleTargetProjectileCallback(ResponsibleSource, Enemy, TotalDamageApplied, HitResult);
		}

		if (BloodHitParticleEffect && PhysicalDamageApplied>0)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BloodHitParticleEffect, Enemy->GetFlipbookComponent()->GetComponentLocation(), FRotator::ZeroRotator, FVector(1));
		}
	} else
	{
		// Ensure the NexusBuilding immune to damage from the projectile
		const AAegisGameStateBase* GameState = Cast<AAegisGameStateBase>(GetWorld()->GetGameState());
		TArray<AActor*> IgnoredActors;
		IgnoredActors.Add(GameState->AegisMap->NexusBuilding);
			
		UGameplayStatics::ApplyRadialDamage(GetWorld(), DamagePackage->PhysicalDamage, OverlappedComponent->GetComponentLocation(), DamagePackage->ExplosionRadius, UPhysicalDamageType::StaticClass(), IgnoredActors);
		UGameplayStatics::ApplyRadialDamage(GetWorld(), DamagePackage->MagicDamage, OverlappedComponent->GetComponentLocation(), DamagePackage->ExplosionRadius, UMagicDamageType::StaticClass(), IgnoredActors);
	}

	// If a particle system is present, spawn the particle system
	if (OnHitParticleSystem)
	{
		UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(OnHitParticleSystem, RootComponent, NAME_None, ProjectileMesh->GetComponentLocation(), FRotator::ZeroRotator, EAttachLocation::Type::KeepRelativeOffset, true);
	}
}

void AProjectileManager::HomingProjectileOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                                      int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitResult)
{
	UStaticMeshComponent* ProjectileMesh = Cast<UStaticMeshComponent>(OverlappedComponent);
	if (!ProjectileMesh) { return; }
	
	// Process hit if the target was an enemy
	if (AEnemy* Enemy = Cast<AEnemy>(OtherActor))
	{
		// Find the relevant projectile package associated with this projectile
		const FHomingProjectilePackage* ProjectilePackage = &ActiveHomingProjectiles[ProjectileMesh];

		ApplyDamageAndEffects(OverlappedComponent, HitResult, ProjectileMesh, Enemy, &ProjectilePackage->DamagePackage, ProjectilePackage->OnHitParticleSystem, ProjectilePackage->ResponsibleSource);
	
		// Mark projectile to be cleaned
		// This must be done at the end of the frame instead of as soon as the overlap event is made, to ensure that it does not break the array loop in Tick()
		ProjectilesToRelease.Add(ProjectileMesh);
	}
}

void AProjectileManager::ArcProjectileOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitResult)
{
	UStaticMeshComponent* ProjectileMesh = Cast<UStaticMeshComponent>(OverlappedComponent);
	if (!ProjectileMesh) { return; }
	
	// Process hit if the target was an enemy
	if (AEnemy* Enemy = Cast<AEnemy>(OtherActor))
	{
		// Find the relevant projectile package associated with this projectile
		const FArcProjectilePackage* ProjectilePackage = &ActiveArcProjectiles[ProjectileMesh];

		ApplyDamageAndEffects(OverlappedComponent, HitResult, ProjectileMesh, Enemy, &ProjectilePackage->DamagePackage, ProjectilePackage->OnHitParticleSystem, ProjectilePackage->ResponsibleSource);
	
		// Mark projectile to be cleaned
		// This must be done at the end of the frame instead of as soon as the overlap event is made, to ensure that it does not break the array loop in Tick()
		ProjectilesToRelease.Add(ProjectileMesh);
	}
}

UStaticMeshComponent* AProjectileManager::LaunchHomingProjectile(const FVector& StartPoint, const AEnemy* TargetEnemy, UStaticMesh* Mesh,
                                                                 const float ProjectileSpeed, const float ProjectileRotationSpeed, const int32 MaxHits, const FProjectileDamagePackage DamagePackage,
                                                                 UObject* ResponsibleSource, UNiagaraSystem* OnHitParticleSystem, UNiagaraSystem* FlightParticleSystem)
{
	// Create a ProjectilePackage 
	const FHomingProjectilePackage ProjectilePackage{
		StartPoint,
		TargetEnemy,
		ProjectileSpeed,
		ProjectileRotationSpeed,
		GetWorld()->GetTimeSeconds(),
		MaxHits,
		DamagePackage,
		ResponsibleSource,
		OnHitParticleSystem,
		FlightParticleSystem};

	// Get a MeshComponent to use from the pool
	UStaticMeshComponent* MeshComponent = AcquireProjectile(ProjectilePackage);

	MeshComponent->SetStaticMesh(Mesh);

	MeshComponent->SetWorldLocation(StartPoint);
	MeshComponent->SetWorldRotation((TargetEnemy->GetActorLocation()-StartPoint).Rotation());
	
	MeshComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &AProjectileManager::HomingProjectileOverlapBegin);
	
	return MeshComponent;
}

UStaticMeshComponent* AProjectileManager::LaunchArcProjectile(const FVector& StartControlPoint, const FVector& CentreControlPoint,
                                                              const FVector& EndControlPoint, UStaticMesh* Mesh, const float StartToEndTravelTime, const FProjectileDamagePackage DamagePackage,
                                                              UObject* ResponsibleSource, UNiagaraSystem* OnHitParticleSystem, UNiagaraSystem* FlightParticleSystem)
{
	// Create a ProjectilePackage 
	FArcProjectilePackage ProjectilePackage{
		StartControlPoint,
		CentreControlPoint,
		EndControlPoint,
		StartToEndTravelTime,
		GetWorld()->GetTimeSeconds(),
		DamagePackage,
		ResponsibleSource,
		OnHitParticleSystem,
		FlightParticleSystem};

	// Get a MeshComponent to use from the pool
	UStaticMeshComponent* MeshComponent = AcquireProjectile(ProjectilePackage);

	MeshComponent->SetStaticMesh(Mesh);
	
	MeshComponent->SetWorldLocation(StartControlPoint);
	MeshComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &AProjectileManager::ArcProjectileOverlapBegin);
	
	return MeshComponent;
}



UStaticMeshComponent* AProjectileManager::AcquireProjectile(const FHomingProjectilePackage& ProjectilePackage)
{
	UStaticMeshComponent* Projectile;

	// If there is an available projectile, take it. Otherwise, create a new one.
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

	// Add the projectile to the relevant data array
	ActiveHomingProjectiles.Add(Projectile, ProjectilePackage);

	return Projectile;
}

UStaticMeshComponent* AProjectileManager::AcquireProjectile(const FArcProjectilePackage& ProjectilePackage)
{
	UStaticMeshComponent* Projectile;

	// If there is an available projectile, take it. Otherwise, create a new one.
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

	// Add the projectile to the relevant data array
	ActiveArcProjectiles.Add(Projectile, ProjectilePackage);

	return Projectile;
}

void AProjectileManager::ReleaseProjectile(UStaticMeshComponent* Projectile)
{
	CleanProjectile(Projectile);

	ActiveHomingProjectiles.Remove(Projectile);
	ActiveArcProjectiles.Remove(Projectile);
	
	AvailableProjectiles.Add(Projectile);
}

void AProjectileManager::CleanProjectile(UStaticMeshComponent* Projectile) const
{
	Projectile->SetStaticMesh(nullptr);
	Projectile->OnComponentBeginOverlap.RemoveAll(this);
	Projectile->SetRelativeLocation(FVector::ZeroVector);
}
