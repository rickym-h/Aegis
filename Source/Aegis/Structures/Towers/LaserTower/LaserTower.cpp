// Fill out your copyright notice in the Description page of Project Settings.


#include "LaserTower.h"

#include "NiagaraComponent.h"
#include "Aegis/Enemies/Enemy.h"
#include "Aegis/Enemies/Damage/MagicDamageType.h"
#include "Aegis/Structures/StructureComponents/TileRangeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
ALaserTower::ALaserTower()
{
	PrimaryActorTick.bCanEverTick = true;
	TileRangeComponent = CreateDefaultSubobject<UTileRangeComponent>("Range Component");
	LaserParticleComponent = CreateDefaultSubobject<UNiagaraComponent>("Laser Particle Component");
	LaserParticleComponent->SetupAttachment(SourcePoint);
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

	if (!IsValid(TargetedEnemy))
	{
		// If the enemy has been killed, attempt to retarget to a new enemy
		if (AEnemy* Enemy = TileRangeComponent->GetFrontEnemy(TileRangeComponent->GetAllEnemiesInRange()))
		{
			TryTargetNewEnemy(Enemy);
		}
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

	if (AEnemy* Enemy = TileRangeComponent->GetFrontEnemy(TileRangeComponent->GetAllEnemiesInRange()))
	{
		TryTargetNewEnemy(Enemy);
	} else
	{
		TryStopTargetEnemy(nullptr);
	}
}

// Called when the game starts or when spawned
void ALaserTower::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALaserTower::TryTargetNewEnemy(AEnemy* TargetEnemy)
{
	UE_LOG(LogTemp, Warning, TEXT("ALaserTower::TryTargetNewEnemy"))
	if (TargetedEnemy && IsValid(TargetedEnemy)) return;

	if (!IsValid(TargetEnemy)) return;
	
	TargetedEnemy = TargetEnemy;
	LaserParticleComponent->SetVisibility(true);
}

void ALaserTower::TryStopTargetEnemy(AEnemy* TargetEnemy)
{
	UE_LOG(LogTemp, Warning, TEXT("ALaserTower::TryStopTargetEnemy"))
	if (TileRangeComponent->GetAllEnemiesInRange().Contains(TargetEnemy)) return;
	
	if (TargetedEnemy == TargetEnemy)
	{
		TargetedEnemy = nullptr;
		LaserParticleComponent->SetVisibility(false);
	}
}

void ALaserTower::Tick(float DeltaSeconds)
{
	if (TargetedEnemy)
	{
		PointAtTargetMesh->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(PointAtTargetMesh->GetComponentLocation(), TargetedEnemy->TargetPoint->GetComponentLocation()));
        
        LaserParticleComponent->SetVectorParameter("Beam End", TargetedEnemy->TargetPoint->GetComponentLocation() - PointAtTargetMesh->GetComponentLocation());
	}
}
