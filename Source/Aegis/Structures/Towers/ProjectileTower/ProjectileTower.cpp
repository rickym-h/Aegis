// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileTower.h"

#include "Aegis/Structures/StructureComponents/DefenderRangeComponent.h"
#include "Aegis/Structures/StructureComponents/ProjectileComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
AProjectileTower::AProjectileTower()
{
	RangeComponent = CreateDefaultSubobject<UDefenderRangeComponent>("Range Component");
	RangeComponent->SetupAttachment(SourcePoint);
	
	ProjectileComponent = CreateDefaultSubobject<UProjectileComponent>("Projectile Component");
}

void AProjectileTower::InitProjectileTower(const FProjectileDamagePackage InProjectileDamagePackage, const float InAttackSpeed,
	const float InRangeMetres, UStaticMesh* InProjectileMesh)
{
	if (InAttackSpeed < 0 || InRangeMetres < 0)
	{
		return;
	}
	this->ProjectileDamagePackage = InProjectileDamagePackage;
	this->AttackSpeed = InAttackSpeed;	
	this->ProjectileMesh = InProjectileMesh;

	RangeComponent->OnEnemyEnterRangeDelegate.AddUniqueDynamic(this, &AProjectileTower::TryFireAtEnemy);
	RangeComponent->InitRange(CurrentLocation, InRangeMetres);

	RangeIndicatorDecal->SetWorldScale3D(FVector(1, InRangeMetres, InRangeMetres));
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
		bShotAvailable = false;
		ProjectileComponent->FireCustomProjectile(ProjectileDamagePackage, SourcePoint->GetComponentLocation(), Enemy, 10, ProjectileMesh);

		GetWorld()->GetTimerManager().SetTimer(
			ReloadTimerHandle, // handle to cancel timer at a later time
			this, // the owning object
			&AProjectileTower::ReloadShot, // function to call on elapsed
			1/AttackSpeed, // float delay until elapsed
			false); // looping?
	}

	PointAtTargetMesh->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(PointAtTargetMesh->GetComponentLocation(), Enemy->TargetPoint->GetComponentLocation()));
}

void AProjectileTower::ReloadShot()
{
	bShotAvailable = true;

	if (const AEnemy* Enemy = RangeComponent->GetFrontEnemy(RangeComponent->GetAllEnemiesInRange()))
	{
		TryFireAtEnemy(Enemy);
	}
}
