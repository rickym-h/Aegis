// Fill out your copyright notice in the Description page of Project Settings.


#include "BombTower.h"

#include "Aegis/Structures/StructureComponents/DefenderRangeComponent.h"
#include "Aegis/Structures/StructureComponents/ProjectileComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
ABombTower::ABombTower()
{
	RangeComponent = CreateDefaultSubobject<UDefenderRangeComponent>("Range Component");
	RangeComponent->SetupAttachment(SourcePoint);
	
	ProjectileComponent = CreateDefaultSubobject<UProjectileComponent>("Projectile Component");
}

void ABombTower::InitBombTowerData(const int InRangeInMeters, const float InShotsPerSecond, const float InDamage, const float InExplosionRadius)
{
	if (InRangeInMeters < 0) { return; }
	if (InShotsPerSecond < 0) { return; }
	if (InShotsPerSecond < 0) { return; }
	this->ShotsPerSecond = InShotsPerSecond;
	this->Damage = InDamage;
	this->ExplosionRadius = InExplosionRadius;

	RangeComponent->OnEnemyEnterRangeDelegate.AddUniqueDynamic(this, &ABombTower::TryFireAtEnemy);
	RangeComponent->InitRange(CurrentLocation, InRangeInMeters);

	RangeIndicatorDecal->SetWorldScale3D(FVector(1, InRangeInMeters, InRangeInMeters));
}

// Called when the game starts or when spawned
void ABombTower::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABombTower::TryFireAtEnemy(const AEnemy* Enemy)
{
	if (!Enemy) { return; }

	if (bShotAvailable)
	{
		bShotAvailable = false;
		ProjectileComponent->FireBombProjectileAtEnemy(SourcePoint->GetComponentLocation(), Enemy, Damage, ExplosionRadius);

		GetWorld()->GetTimerManager().SetTimer(
			ReloadTimerHandle, // handle to cancel timer at a later time
			this, // the owning object
			&ABombTower::ReloadShot, // function to call on elapsed
			1/ShotsPerSecond, // float delay until elapsed
			false); // looping?
	}

	PointAtTargetMesh->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(PointAtTargetMesh->GetComponentLocation(), Enemy->TargetPoint->GetComponentLocation()));

}

void ABombTower::ReloadShot()
{
	bShotAvailable = true;

	if (const AEnemy* Enemy = RangeComponent->GetFrontEnemy(RangeComponent->GetAllEnemiesInRange()))
	{
		TryFireAtEnemy(Enemy);
	}
}
