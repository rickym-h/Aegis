// Fill out your copyright notice in the Description page of Project Settings.


#include "AegisGameMap.h"

FTileCoord UAegisGameMap::GetEnemySpawnCoord() const
{
	if (PathStartTiles.Num() == 1) { return PathStartTiles[0]; }

	const int RandomIndex = FMath::RandRange(0, PathStartTiles.Num() - 1);
	//UE_LOG(LogTemp, Warning, TEXT("Index: %i"), RandomIndex)
	return PathStartTiles[RandomIndex];
}

FTileCoord UAegisGameMap::GetNextCoordInPath(const FTileCoord CurrentCoord) const
{
	return PathRoute[CurrentCoord];
}

int UAegisGameMap::GetNumOfTilesToEnd(const FTileCoord StartCoord)
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

bool UAegisGameMap::IsTileAvailable(const FTileCoord& Location) const
{
	if (!Super::IsTileAvailable(Location)) { return false; }
	
	// Check the location is not part of the path
	if (PathRoute.Contains(Location)) { return false; }

	return true;
}

void UAegisGameMap::PopulateGameMapData(const TMap<FTileCoord, UMapTileData*>& InMapTileData, const TMap<FTileCoord, FTileCoord>& InPath,
	const TArray<FTileCoord>& InPathStartCoords, ANexusBuilding* InNexusBuilding)
{
	PopulateMapData(InMapTileData);

	this->PathRoute = InPath;
	this->PathStartTiles = InPathStartCoords;
	this->NexusBuilding = InNexusBuilding;
}
