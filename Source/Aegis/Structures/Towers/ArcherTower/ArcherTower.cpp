// Fill out your copyright notice in the Description page of Project Settings.


#include "ArcherTower.h"

#include "Aegis/Structures/StructureComponents/DefenderRangeComponent.h"
#include "Aegis/Structures/StructureComponents/ProjectileComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/KismetMathLibrary.h"

AArcherTower::AArcherTower()
{
	RangeComponent = CreateDefaultSubobject<UDefenderRangeComponent>("Range Component");
	RangeComponent->SetupAttachment(SourcePoint);
	
	ProjectileComponent = CreateDefaultSubobject<UProjectileComponent>("Projectile Component");
}

void AArcherTower::InitArcherTowerData(const int InRangeInMeters, const float InShotsPerSecond, const float InDamage)
{
	if (InRangeInMeters < 0) { return; }
	if (InShotsPerSecond < 0) { return; }
	this->ShotsPerSecond = InShotsPerSecond;
	this->Damage = InDamage;

	RangeComponent->OnEnemyEnterRangeDelegate.AddUniqueDynamic(this, &AArcherTower::TryFireAtEnemy);
	RangeComponent->InitRange(CurrentLocation, InRangeInMeters);

	RangeIndicatorDecal->SetWorldScale3D(FVector(1, InRangeInMeters, InRangeInMeters));
}

void AArcherTower::BeginPlay()
{
	Super::BeginPlay();
}

void AArcherTower::TryFireAtEnemy(const AEnemy* Enemy)
{
	if (!Enemy) { return; }

	if (bShotAvailable)
	{
		bShotAvailable = false;
		ProjectileComponent->FireArrowAtEnemy(SourcePoint->GetComponentLocation(), Enemy, Damage);

		GetWorld()->GetTimerManager().SetTimer(
			ReloadTimerHandle, // handle to cancel timer at a later time
			this, // the owning object
			&AArcherTower::ReloadShot, // function to call on elapsed
			1/ShotsPerSecond, // float delay until elapsed
			false); // looping?
	}

	PointAtTargetMesh->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(PointAtTargetMesh->GetComponentLocation(), Enemy->TargetPoint->GetComponentLocation()));
}

void AArcherTower::ReloadShot()
{
	bShotAvailable = true;

	if (AEnemy* Enemy = RangeComponent->GetFrontEnemy(RangeComponent->GetAllEnemiesInRange()))
	{
		TryFireAtEnemy(Enemy);
	}
}
