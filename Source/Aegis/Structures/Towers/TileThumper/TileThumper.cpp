// Fill out your copyright notice in the Description page of Project Settings.


#include "TileThumper.h"

#include "Aegis/Structures/StructureComponents/TileRangeComponent.h"


// Sets default values
ATileThumper::ATileThumper()
{
	RangeComponent = CreateDefaultSubobject<UTileRangeComponent>("Range Component");
}

void ATileThumper::InitTileThumper(const float InAttackSpeed, const TSet<FTileCoord> TileRangeOffsets)
{
	if (InAttackSpeed < 0 || TileRangeOffsets.Num() < 0)
	{
		UE_LOG(LogTemp, Error, TEXT("ATileThumper::InitTileThumper - InAttackSpeed or TileRangeOffsets.Num not valid!"))
		return;
	}
	this->AttackSpeed = InAttackSpeed;

	RangeComponent->OnEnemyEnterRangeDelegate.AddUniqueDynamic(this, &ATileThumper::TryThump);
	RangeComponent->InitRange(CurrentLocation, TileRangeOffsets);
}

// Called when the game starts or when spawned
void ATileThumper::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATileThumper::TryThump(const AEnemy* Enemy)
{
	if (!Enemy) { return; }

	if (bCanThump)
	{
		bCanThump = false;

		UE_LOG(LogTemp, Warning, TEXT("ATileThumper::TryThump - THUMP!!!!"))
		
		GetWorld()->GetTimerManager().SetTimer(
			RechargeTimerHandle, // handle to cancel timer at a later time
			this, // the owning object
			&ATileThumper::RechargeThump, // function to call on elapsed
			1/AttackSpeed, // float delay until elapsed
			false); // looping?
	}
}

void ATileThumper::RechargeThump()
{
	bCanThump = true;

	if (const AEnemy* Enemy = RangeComponent->GetFrontEnemy(RangeComponent->GetAllEnemiesInRange()))
	{
		TryThump(Enemy);
	}
}
