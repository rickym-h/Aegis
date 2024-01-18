// Fill out your copyright notice in the Description page of Project Settings.


#include "AegisMap.h"

UAegisMap::UAegisMap()
{
}

void UAegisMap::PopulateMapData(
	const TMap<FTileCoord, AMapTile*>& InMapTiles,
	const TMap<FTileCoord, FTileCoord>& InPathRoute,
	const TArray<FTileCoord>& InPathStartTiles,
	ANexusBuilding* InNexusBuilding)
{
	this->MapTiles = InMapTiles;
	this->PathRoute = InPathRoute;
	this->PathStartTiles = InPathStartTiles;
	this->NexusBuilding = InNexusBuilding;
	UE_LOG(LogTemp, Warning, TEXT("UAegisMap::PopulateMapData()"))
}

bool UAegisMap::IsCoordInPath(const FTileCoord Coord) const
{
	return PathRoute.Contains(Coord);
}

AMapTile* UAegisMap::GetTile(const FTileCoord Coord)
{
	return *MapTiles.Find(Coord);
}

FTileCoord UAegisMap::GetEnemySpawnCoord() const
{
	UE_LOG(LogTemp, Warning, TEXT("UAegisMap::GetEnemySpawnCoord()"))
	//return FTileCoord(9, 0);

	for (FTileCoord Coord : PathStartTiles)
	{
		UE_LOG(LogTemp, Warning, TEXT("%ls"), *Coord.ToString())
	}
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
