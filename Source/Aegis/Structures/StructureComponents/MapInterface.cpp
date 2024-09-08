// Fill out your copyright notice in the Description page of Project Settings.


#include "MapInterface.h"

#include "Aegis/Cards/StructureCard.h"
#include "Aegis/Core/GameStates/AegisGameStateBase.h"
#include "Aegis/Map/AegisGameMap.h"
#include "Aegis/Structures/Structure.h"
#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
UMapInterface::UMapInterface()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

int32 UMapInterface::CountResourcesInRange(const EResourceType ResourceType, const int32 RangeInTiles, const bool IncludeSelfTile = false)
{
	TArray<FTileCoord> CoordsInRange = FTileCoord::GetTilesInRadius(OwnerCoords, RangeInTiles);

	if (!IncludeSelfTile)
	{
		for (const FTileCoord OwnerCoord : OwnerCoords)
		{
			CoordsInRange.Remove(OwnerCoord);
		}
	}

	int32 Count = 0;
	for (const FTileCoord Coord : CoordsInRange)
	{
		const UMapTileData* MapTileData = Map->GetTileData(Coord);
		if (!MapTileData) { continue; }

		if (MapTileData->ResourceType == ResourceType)
		{
			Count++;
		}
	}

	return Count;
}


// Called when the game starts
void UMapInterface::BeginPlay()
{
	Super::BeginPlay();

	SetInternalData();
}

void UMapInterface::SetInternalData()
{
	if (const AAegisGameStateBase* GameState = Cast<AAegisGameStateBase>(UGameplayStatics::GetGameState(GetWorld())))
	{
		Map = Cast<UAegisGameMap>(GameState->AegisMap);
	} else
	{
		UE_LOG(LogTemp, Error, TEXT("UMapInterface::BeginPlay - Map is NULL"));
	}

	const AStructure* Owner = Cast<AStructure>(GetOwner());
	const FTileCoord OwnerOrigin = Owner->CurrentLocation;

	for (FTileCoord Offset : Owner->StructureCard->StructureOffsets)
	{
		OwnerCoords.Add(OwnerOrigin + Offset);
	}
}
