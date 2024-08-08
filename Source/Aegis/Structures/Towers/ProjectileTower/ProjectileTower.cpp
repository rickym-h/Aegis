// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileTower.h"

#include "Aegis/AegisGameStateBase.h"
#include "Aegis/Enemies/Enemy.h"
#include "Aegis/Structures/StructureComponents/DefenderRangeComponent.h"
#include "Aegis/Utilities/ProjectileManager.h"
#include "Components/DecalComponent.h"
#include "Kismet/KismetMathLibrary.h"

AProjectileTower::AProjectileTower()
{
	RangeComponent = CreateDefaultSubobject<UDefenderRangeComponent>("Range Component");
	RangeComponent->SetupAttachment(SourcePoint);
	ShotAvailable = true;

	
	
}

void AProjectileTower::TryFireAtEnemy(const AEnemy* Enemy)
{
	if (!Enemy) { return; }

	if (ShotAvailable)
	{
		ShotAvailable = false;
		FireProjectileAtEnemy(Enemy);

		GetWorld()->GetTimerManager().SetTimer(
			ReloadTimerHandle, // handle to cancel timer at a later time
			this, // the owning object
			&AProjectileTower::ReloadShot, // function to call on elapsed
			1/AttackSpeed, // float delay until elapsed
			false); // looping?
	}

	PointAtTargetMesh->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(PointAtTargetMesh->GetComponentLocation(), Enemy->TargetPoint->GetComponentLocation()));
}

void AProjectileTower::FireProjectileAtEnemy(const AEnemy* Enemy)
{
	if (const AAegisGameStateBase* GameState = Cast<AAegisGameStateBase>(GetWorld()->GetGameState()))
	{
		const FVector Start = SourcePoint->GetComponentLocation();
		GameState->ProjectileManager->FireProjectile(DamagePackage, Start, Enemy, ProjectileSpeed, ProjectileMesh);
	}
}

void AProjectileTower::ReloadShot()
{
	ShotAvailable = true;

	if (AEnemy* Enemy = RangeComponent->GetFrontEnemy(RangeComponent->GetAllEnemiesInRange()))
	{
		TryFireAtEnemy(Enemy);
	}
}

void AProjectileTower::SetTowerRange(const int NewRange)
{
	if (NewRange <= 0) { return; }

	TowerRange = NewRange;
	RangeComponent->OnEnemyEnterRangeDelegate.AddUniqueDynamic(this, &AProjectileTower::TryFireAtEnemy);
	RangeComponent->InitRange(CurrentLocation, NewRange);

	const int DecalScale = 1+(TowerRange*2);
	RangeIndicatorDecal->SetWorldScale3D(FVector(1, DecalScale, DecalScale));
}
