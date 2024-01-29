// Fill out your copyright notice in the Description page of Project Settings.


#include "DefenderRangeComponent.h"

#include "Aegis/AegisGameStateBase.h"
#include "Aegis/Map/AegisMap.h"
#include "Aegis/Map/MapTile.h"

// Sets default values for this component's properties
UDefenderRangeComponent::UDefenderRangeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UDefenderRangeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UDefenderRangeComponent::OnEnemyEnterRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("ENEMY ENTERED RANGE"))
}

void UDefenderRangeComponent::InitRange(const FTileCoord DefenderCoord, const int Range)
{
	const AAegisGameStateBase* GameState = Cast<AAegisGameStateBase>(GetWorld()->GetGameState());
	if (!GameState) { return; }
	
	for (const FTileCoord TileInRange : FTileCoord::GetTilesInRadius(DefenderCoord, Range))
	{
		// Get Tile
		AMapTile* Tile = GameState->AegisMap->GetTile(TileInRange);

		// Add tile to our TilesInRange TArray
		TilesInRange.Add(Tile);
		
		// Subscribe Enemy Entered range function to Overlap delegate in Tile Collision Mesh
		Tile->CollisionVolume->OnComponentBeginOverlap.AddUniqueDynamic(this, &UDefenderRangeComponent::OnEnemyEnterRange);
	}
}

