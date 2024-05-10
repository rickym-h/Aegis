// Fill out your copyright notice in the Description page of Project Settings.


#include "StructureData.h"

#include "Aegis/AegisGameStateBase.h"
#include "Aegis/Map/AegisMap.h"

UStaticMesh* UStructureData::GetMeshRepresentation() const
{
	return MeshRepresentation;
}

TSubclassOf<AStructure> UStructureData::GetStructureBlueprintClass() const
{
	return StructureBlueprintClass;
}

bool UStructureData::CanStructureBePlaced(const FTileCoord Location)
{
	// TODO add check for player resources too
	return IsTileTypeValid(Location);
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
