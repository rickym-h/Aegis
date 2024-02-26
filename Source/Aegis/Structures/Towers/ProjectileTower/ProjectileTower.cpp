// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileTower.h"

#include "Aegis/Enemies/Enemy.h"
#include "Aegis/Structures/StructureComponents/DefenderRangeComponent.h"

AProjectileTower::AProjectileTower()
{
	RangeComponent = CreateDefaultSubobject<UDefenderRangeComponent>("Range Component");
	ShotAvailable = true;
}


void AProjectileTower::TryFireAtEnemy(AEnemy* Enemy)
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
			1.0f, // float delay until elapsed
			false); // looping?
	}
}

void AProjectileTower::FireProjectileAtEnemy(AEnemy* Enemy)
{	
	// TODO actually fire projectile
	const FVector Start = GetActorLocation() + FVector(0,0,100);
	const FVector End = Enemy->GetActorLocation() + FVector(0,0,100);
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1, 0, 5);
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
	if (NewRange<=0) { return; }

	TowerRange = NewRange;
	RangeComponent->InitRange(CurrentLocation, NewRange);

	RangeComponent->OnEnemyEnterRangeDelegate.AddUniqueDynamic(this, &AProjectileTower::TryFireAtEnemy);
}
 