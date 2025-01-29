// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileTower.h"

#include "Aegis/Structures/StructureComponents/ProjectileComponent.h"
#include "Aegis/Structures/StructureComponents/TileRangeComponent.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
AProjectileTower::AProjectileTower()
{
	TileRangeComponent = CreateDefaultSubobject<UTileRangeComponent>("Range Component");
	
	ProjectileComponent = CreateDefaultSubobject<UProjectileComponent>("Projectile Component");
}

void AProjectileTower::InitProjectileTower(const FProjectileDamagePackage InProjectileDamagePackage, const float InAttackSpeed,
	const int32 InRangeTiles, UStaticMesh* InProjectileMesh)
{
	if (InAttackSpeed < 0 || InRangeTiles < 0)
	{
		return;
	}
	this->ProjectileDamagePackage = InProjectileDamagePackage;
	this->AttackSpeed = InAttackSpeed;	
	this->ProjectileMesh = InProjectileMesh;

	TileRangeComponent->OnEnemyEnterRangeDelegate.AddUniqueDynamic(this, &AProjectileTower::TryFireAtEnemy);
	TileRangeComponent->InitRange(CurrentLocation, InRangeTiles);
}

// Called when the game starts or when spawned
void AProjectileTower::BeginPlay()
{
	Super::BeginPlay();
	
}

void AProjectileTower::TryFireAtEnemy(const AEnemy* Enemy)
{
	if (!Enemy) { return; }

	if (bShotAvailable)
	{
		PointAtTargetMesh->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(PointAtTargetMesh->GetComponentLocation(), Enemy->TargetPoint->GetComponentLocation()));
		
		bShotAvailable = false;
		ProjectileComponent->FireArrow(SourcePoint->GetComponentLocation(), Enemy, 1);

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

	if (const AEnemy* Enemy = TileRangeComponent->GetFrontEnemy(TileRangeComponent->GetAllEnemiesInRange()))
	{
		TryFireAtEnemy(Enemy);
	}
}
