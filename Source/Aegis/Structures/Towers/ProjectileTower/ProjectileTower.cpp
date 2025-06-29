// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileTower.h"

#include "NiagaraFunctionLibrary.h"
#include "PaperFlipbookComponent.h"
#include "Aegis/Enemies/Components/StatusEffectComponent.h"
#include "Aegis/Structures/StructureComponents/ProjectileComponent.h"
#include "Aegis/Structures/StructureComponents/TileRangeComponent.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
AProjectileTower::AProjectileTower()
{
	TileRangeComponent = CreateDefaultSubobject<UTileRangeComponent>("Range Component");
	
	ProjectileComponent = CreateDefaultSubobject<UProjectileComponent>("Projectile Component");
}

void AProjectileTower::InitProjectileTower(EProjectileType InProjectileType, const FProjectileDamagePackage InProjectileDamagePackage,
	UStaticMesh* InProjectileMesh, const float InAttackSpeed, const int32 InRangeTiles, const FSlowEffect InSlowEffect,
	const FPoisonEffect InPoisonEffect)
{
	if (InAttackSpeed < 0 || InRangeTiles < 0)
	{
		return;
	}
	this->ProjectileType = InProjectileType;
	this->CustomProjectileProjectileDamagePackage = InProjectileDamagePackage;
	this->AttackSpeed = InAttackSpeed;	
	this->ProjectileMesh = InProjectileMesh;

	TileRangeComponent->OnEnemyEnterRangeDelegate.AddUniqueDynamic(this, &AProjectileTower::TryFireAtEnemy);
	TileRangeComponent->InitRange(CurrentLocation, InRangeTiles);
	
	SlowEffect = InSlowEffect;
	PoisonEffect = InPoisonEffect;
}

// Called when the game starts or when spawned
void AProjectileTower::BeginPlay()
{
	Super::BeginPlay();
	
}

void AProjectileTower::TryFireAtEnemy(AEnemy* Enemy)
{
	if (!Enemy) { return; }

	if (bShotAvailable)
	{
		PointAtTargetMesh->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(PointAtTargetMesh->GetComponentLocation(), Enemy->TargetPoint->GetComponentLocation()));
		
		bShotAvailable = false;
		switch (ProjectileType)
		{
		case EProjectileType::ArcArrow:
			ProjectileComponent->FireArcArrow(SourcePoint->GetComponentLocation(), Enemy, 1);
			break;
		case EProjectileType::RunicSpark:
			ProjectileComponent->FireRunicSpark(SourcePoint->GetComponentLocation(), Enemy, 1);
			break;
		case EProjectileType::CustomArcProjectile:
			ProjectileComponent->FireSimpleArcProjectile(SourcePoint->GetComponentLocation(), Enemy->TargetPoint->GetComponentLocation(), CustomProjectileProjectileDamagePackage, 40, ProjectileMesh);
			break;
		case EProjectileType::CustomHomingProjectile:
			UE_LOG(LogTemp, Error, TEXT("Firing Custom Homing Projectile - NOT IMPLEMENTED"))
			break;
		default:
			break;
		}
		

		GetWorld()->GetTimerManager().SetTimer(
			ReloadTimerHandle, // handle to cancel timer at a later time
			this, // the owning object
			&AProjectileTower::ReloadShot, // function to call on elapsed
			1/AttackSpeed, // float delay until elapsed
			false); // looping?
	}
}

void AProjectileTower::ReloadShot()
{
	bShotAvailable = true;

	if (AEnemy* Enemy = TileRangeComponent->GetFrontEnemy(TileRangeComponent->GetAllEnemiesInRange()))
	{
		TryFireAtEnemy(Enemy);
	}
}

void AProjectileTower::SingleTargetProjectileCallback_Implementation(AEnemy* HitEnemy, const float DamageApplied, const FHitResult& HitResult)
{
	// It is possible that the enemy has been marked for garbage collection if they have already "died", so check before applying status effects.
	if (IsValid(HitEnemy))
	{
		HitEnemy->GetStatusEffectComponent()->ApplySlowEffect(SlowEffect);
		HitEnemy->GetStatusEffectComponent()->ApplyPoisonStacks(PoisonEffect);
	}

	if (OnHitEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), OnHitEffect, HitEnemy->GetFlipbookComponent()->GetComponentLocation(), FRotator::ZeroRotator, FVector(1));
	}
}
