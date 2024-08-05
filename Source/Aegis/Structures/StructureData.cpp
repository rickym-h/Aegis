// Fill out your copyright notice in the Description page of Project Settings.


#include "StructureData.h"

#include "Aegis/AegisGameStateBase.h"
#include "Aegis/Map/AegisMap.h"
#include "Aegis/Map/MapTile.h"
#include "Aegis/Pawns/AegisPlayerController.h"
#include "Kismet/GameplayStatics.h"


TSubclassOf<AStructure> UStructureData::GetStructureBlueprintClass() const
{
	return StructureBlueprintClass;
}

bool UStructureData::CanStructureBePlaced(const FTileCoord Location, const bool bCheckSingleTile)
{
	if (!Cast<AAegisPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->Resources->IsResourcesEnough(StructureCost))
	{
		return false;
	}
	
	if (!MapReference)
	{
		MapReference = Cast<AAegisGameStateBase>(GetWorld()->GetGameState())->AegisMap;
	}	

	// Check that all the offset tiles are valid and at same elevation as origin tile
	const int32 OriginTileElevation = MapReference->GetTile(Location)->MapTileData->Elevation;
	if (!bCheckSingleTile)
	{
		for(FTileCoord Offset : StructureOffsets)
		{
			const AMapTile* OffsetTile = MapReference->GetTile(Location + Offset);
			if (!OffsetTile || OffsetTile->MapTileData->Elevation != OriginTileElevation)
			{
				return false;
			}
			
			if (!IsTileTypeValid(Location + Offset))
			{
				return false;
			}
		}
	}
	// Check origin tile is valid because the offset tiles do not include the origin tile
	if (!IsTileTypeValid(Location))
	{
		return false;
	}

	
	return true;
}

bool UStructureData::IsTileTypeValid(const FTileCoord Location) const
{
	// Check there are no other structures there
	if (!MapReference->IsTileAvailable(Location)) { return false; }

	// Check the terrain type and resource type is valid
	const ETerrainType TileTerrain = MapReference->GetTileData(Location)->TerrainType;
	const EResourceType TileResource = MapReference->GetTileData(Location)->ResourceType;

	return AllowedTerrains.Contains(TileTerrain) && AllowedResources.Contains(TileResource);
}
