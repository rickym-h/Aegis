// Fill out your copyright notice in the Description page of Project Settings.


#include "LaserTower.h"

#include "Aegis/Enemies/Enemy.h"
#include "Aegis/Enemies/Damage/MagicDamageType.h"
#include "Aegis/Structures/StructureComponents/TileRangeComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ALaserTower::ALaserTower()
{
	TileRangeComponent = CreateDefaultSubobject<UTileRangeComponent>("Range Component");

	
}

void ALaserTower::TryDealDamage()
{
	if (TargetedEnemy)
	{
		UGameplayStatics::ApplyDamage(
			TargetedEnemy,
			DamagePerSecond / DamageFireRate,
			UGameplayStatics::GetPlayerController(GetWorld(), 0), this, UMagicDamageType::StaticClass());
	}
}

void ALaserTower::InitLaserTower(const int32 InRangeTiles, const float InDamagePerSecond, const float InDamageFireRate)
{
	TileRangeComponent->OnEnemyEnterRangeDelegate.AddUniqueDynamic(this, &ALaserTower::TryTargetNewEnemy);
	TileRangeComponent->OnEnemyLeaveRangeDelegate.AddUniqueDynamic(this, &ALaserTower::TryStopTargetEnemy);
	TileRangeComponent->InitRange(CurrentLocation, InRangeTiles);

	DamagePerSecond = InDamagePerSecond;
	DamageFireRate = InDamageFireRate;

	FTimerHandle LaserDamageTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		LaserDamageTimerHandle,
		this,
		&ALaserTower::TryDealDamage,
		1/DamageFireRate,
		true,
		0
		);
}

// Called when the game starts or when spawned
void ALaserTower::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALaserTower::TryTargetNewEnemy(AEnemy* TargetEnemy)
{
	if (TargetedEnemy) return;

	TargetedEnemy = TargetEnemy;
	
}

void ALaserTower::TryStopTargetEnemy(AEnemy* TargetEnemy)
{
	if (TargetedEnemy == TargetEnemy)
	{
		TargetedEnemy = nullptr;
	}
}
