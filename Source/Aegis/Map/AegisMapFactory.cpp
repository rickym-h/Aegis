// Fill out your copyright notice in the Description page of Project Settings.


#include "AegisMapFactory.h"

#include "MapTile.h"
#include "PathGenerationBlueprintLibrary.h"
#include "Aegis/Structures/NexusBuilding/NexusBuilding.h"

void UAegisMapFactory::PostInitProperties()
{
	UObject::PostInitProperties();

	if (!GetWorld()) { return; }
	
	TileHeight = 2*TileRadius;
	TileWidth = FMath::Sqrt(3.f) * TileRadius;

	VerticalSpacing = 1.5 * TileRadius;
	HorizontalSpacing = TileWidth;

	OffsetQ = FVector(0, HorizontalSpacing, 0);
	OffsetR = FVector(VerticalSpacing, HorizontalSpacing/2, 0);
}

UAegisMap* UAegisMapFactory::GenerateMapWithNoise(const int MainPathLength) const
{
	const FRandomStream RandomStream = FRandomStream(static_cast<int32>(FDateTime::Now().ToUnixTimestamp()));
	const FVector2D ElevationNoiseOffset = FVector2D(RandomStream.FRandRange(-100000.f, 100000.f), RandomStream.FRandRange(-100000.f, 100000.f));
	const FVector2D PathingNoiseOffset = FVector2D(RandomStream.FRandRange(-100000.f, 100000.f), RandomStream.FRandRange(-100000.f, 100000.f));
	const FVector2D TreeNoiseOffset = FVector2D(RandomStream.FRandRange(-100000.f, 100000.f), RandomStream.FRandRange(-100000.f, 100000.f));
	const FVector2D StoneNoiseOffset = FVector2D(RandomStream.FRandRange(-100000.f, 100000.f), RandomStream.FRandRange(-100000.f, 100000.f));
	
	// Generate a Path - this is done using a Greedy search through some Poisson Blue Noise
	const TMap<FTileCoord, FTileCoord> Path = UPathGenerationBlueprintLibrary::GenerateGreedyPoissonPath(MainPathLength, PathingNoiseOffset, RandomStream);

	const TMap<FTileCoord, UMapTileData*> MapTilesData = UPathGenerationBlueprintLibrary::GenerateMapTilesData(Path, ElevationNoiseOffset, TreeNoiseOffset, StoneNoiseOffset, RandomStream);
	
	// Add nexus building
	ANexusBuilding* NexusBuilding = GetWorld()->SpawnActor<ANexusBuilding>(NexusBuildingBP, FVector(0,0,0), FRotator(0,0,0));

	// Set path start tile coords
	const TArray<FTileCoord> PathStartTileCoords = GetPathStartCoords(Path);

	// Generate tiles - setting tile heights, and tile types

	// Create Map instance
	UAegisMap* Map = NewObject<UAegisMap>(GetWorld(), AegisMapClass);	
	Map->PopulateMapData(MapTilesData, Path, PathStartTileCoords, NexusBuilding);

	// Set TilesToEnd of every tile
	// Post Map creation settings
	for (AMapTile* Tile : Map->GetTiles())
	{
		Tile->TilesToEnd = Map->GetNumOfTilesToEnd(Tile->TileCoord);
	}
	
	return Map;
}

TArray<FTileCoord> UAegisMapFactory::GetPathStartCoords(TMap<FTileCoord, FTileCoord> PathRoute)
{
	// Get all coords where no other tiles points into it
	TSet<FTileCoord> PathStartCoords;
	
	// Get all starting coords
	for (const TTuple<FTileCoord, FTileCoord> Pair : PathRoute)
	{
		PathStartCoords.Add(Pair.Key);
	}
	// Remove all coords which are pointed to
	for (const TTuple<FTileCoord, FTileCoord> Pair : PathRoute)
	{
		PathStartCoords.Remove(Pair.Value);
	}
	return PathStartCoords.Array();
}
