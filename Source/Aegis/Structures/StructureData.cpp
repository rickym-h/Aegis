// Fill out your copyright notice in the Description page of Project Settings.


#include "StructureData.h"

#include "Aegis/AegisGameStateBase.h"
#include "Aegis/Map/AegisMap.h"
#include "Aegis/Pawns/PlayerPawn.h"
#include "Kismet/GameplayStatics.h"


TSubclassOf<AStructure> UStructureData::GetStructureBlueprintClass() const
{
	return StructureBlueprintClass;
}

bool UStructureData::CanStructureBePlaced(const FTileCoord Location)
{
	return IsTileTypeValid(Location) && Cast<APlayerPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0))->Resources->IsResourcesEnough(StructureCost);
}

bool UStructureData::IsTileTypeValid(const FTileCoord Location)
{
	if (!MapReference)
	{
		MapReference = Cast<AAegisGameStateBase>(GetWorld()->GetGameState())->AegisMap;
	}

	// Check there are no other structures there
	if (!MapReference->IsTileAvailable(Location)) { return false; }

	// Check the terrain type and resource type is valid
	const ETerrainType TileTerrain = MapReference->GetTileData(Location)->TerrainType;
	const EResourceType TileResource = MapReference->GetTileData(Location)->ResourceType;

	return AllowedTerrains.Contains(TileTerrain) && AllowedResources.Contains(TileResource);
}
