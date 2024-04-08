// Fill out your copyright notice in the Description page of Project Settings.


#include "AegisMap.h"

#include "MapTile.h"
#include "Aegis/Structures/Structure.h"
#include "Aegis/Structures/Towers/Tower.h"
#include "Aegis/Structures/Towers/TowerData.h"
#include "MapTiles/MapTileData.h"


UAegisMap::UAegisMap()
{
}

void UAegisMap::PopulateMapData(
	const TMap<FTileCoord, UMapTileData*>& InMapTileData,
	const TMap<FTileCoord, FTileCoord>& InPathRoute,
	const TArray<FTileCoord>& InPathStartTiles,
	ANexusBuilding* InNexusBuilding)
{	
	this->MapTiles = GenerateMapTiles(InMapTileData);
	this->PathRoute = InPathRoute;
	this->PathStartTiles = InPathStartTiles;
	this->NexusBuilding = InNexusBuilding;
}

bool UAegisMap::IsCoordInPath(const FTileCoord Coord) const
{
	return PathRoute.Contains(Coord);
}

AMapTile* UAegisMap::GetTile(const FTileCoord Coord)
{
	// For some reason TMap::Find throws an error, so we manually check if is contained and then return the value.
	if (MapTiles.Contains(Coord))
	{
		return MapTiles[Coord];
	}
	return nullptr;
}

TArray<AMapTile*> UAegisMap::GetTiles()
{
	TArray<AMapTile*> Tiles;
	for (const TPair<FTileCoord, AMapTile*>& Entry : MapTiles)
	{
		Tiles.Add(Entry.Value);
	}
	return Tiles;
}

FTileCoord UAegisMap::GetEnemySpawnCoord() const
{
	if (PathStartTiles.Num() == 1) { return PathStartTiles[0]; }
	
	const int RandomIndex = FMath::RandRange(0, PathStartTiles.Num()-1);
	//UE_LOG(LogTemp, Warning, TEXT("Index: %i"), RandomIndex)
	return PathStartTiles[RandomIndex];
}

FTileCoord UAegisMap::GetNextCoordInPath(const FTileCoord CurrentCoord) const
{
	//return FTileCoord(8, 0);
	return PathRoute[CurrentCoord];
}

bool UAegisMap::AddStructureToMap(const FTileCoord Location, UStructureData* StructureData)
{
	// Check the location is valid And resources are valid
	if (!IsTileAvailable(Location)) { return false; }

	// Get the tower class needed
	if (!StructureData->GetStructureBlueprintClass()) { return false; }

	// Create actor instance of tower class
	// Set the data of the tower actor based on tower data
	// Finish spawning tower actor
	AStructure* Structure = StructureData->SpawnStructureFromData(Location, GetTile(Location)->StructureLocation);
	
	if (!Structure) { return false; }
	//Tower->TowerData = TowerData;
	
	MapStructures.Add(Location,	Structure);
	
	// TODO Take any resources needed

	return true;
}

int UAegisMap::GetNumOfTilesToEnd(const FTileCoord StartCoord)
{
	int DistanceSoFar = 0;
	FTileCoord CurrentCoord = StartCoord;
	while (PathRoute.Contains(CurrentCoord))
	{
		FTileCoord NextCoord = PathRoute[CurrentCoord];
		if (CurrentCoord == NextCoord) { break; }

		DistanceSoFar++;
		CurrentCoord = NextCoord;
	}
	return DistanceSoFar;
}

AMapTile* UAegisMap::CreateMapTile(const FTileCoord Coord, UMapTileData* MapTileData) const
{
	const FVector Location = Coord.ToWorldLocation();
	const FRotator Rotation = FRotator::ZeroRotator;
	
	AMapTile* Tile = GetWorld()->SpawnActor<AMapTile>(MapTileBP, Location, Rotation);
	Tile->TileCoord = Coord.Copy();

	Tile->SetMapTileData(MapTileData);
	
	return Tile;
}

TMap<FTileCoord, AMapTile*> UAegisMap::GenerateMapTiles(const TMap<FTileCoord, UMapTileData*>& MapTileData) const
{
	TMap<FTileCoord, AMapTile*> OutMapTiles;
	
	for (TTuple<FTileCoord, UMapTileData*> Element : MapTileData)
	{
		AMapTile* MapTile = CreateMapTile(Element.Key, Element.Value);
		OutMapTiles.Add(Element.Key, MapTile);		
	}
	
	return OutMapTiles;
}


bool UAegisMap::IsTileAvailable(const FTileCoord Location) const
{
	if (MapStructures.Contains(Location)) { return false; }

	if (PathRoute.Contains(Location)) { return false; }
	
	return true;
}
