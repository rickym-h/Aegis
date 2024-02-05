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

bool UAegisMap::AddTowerToMap(FTileCoord Location, UTowerData* TowerData)
{
	return false;

	// Check the location is valid And resources are valid

	// Get the tower class needed

	// Create actor instance of tower class

	// Set the data of the tower actor based on tower data

	// Finish spawning tower actor

	// Add towerdata to map
	
	// Take any resources needed

	// Return true
}

bool UAegisMap::AddDefenderToMap(const FTileCoord Location)
{
	if (!IsTileAvailable(Location)) { return false; }

	// if (!DefaultDefender) { return false; }

	// const FTransform ActorTransform = FTransform(Location.ToWorldLocation());
	// ABaseProjectileTower* NewDefender = GetWorld()->SpawnActorDeferred<ABaseProjectileTower>(DefaultDefender, ActorTransform);
	//
	// NewDefender->CurrentLocation = Location;
	// NewDefender->TowerRange = 2;
	//
	// UGameplayStatics::FinishSpawningActor(NewDefender, ActorTransform);
	//
	// MapDefenders.Add(Location, NewDefender);

	return true;
}

bool UAegisMap::IsTileAvailable(FTileCoord Location)
{
	// if (MapDefenders.Contains(Location)) { return false; }

	// TODO implement for gaia too
	return true;
}
