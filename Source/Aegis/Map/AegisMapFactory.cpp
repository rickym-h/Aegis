// Fill out your copyright notice in the Description page of Project Settings.


#include "AegisMapFactory.h"

#include "IContentBrowserSingleton.h"
#include "MapTile.h"
#include "PathGenerationBlueprintLibrary.h"
#include "Aegis/Structures/NexusBuilding.h"
#include "Kismet/BlueprintPathsLibrary.h"

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
	
	FRandomStream RandomStream = FRandomStream(static_cast<int32>(FDateTime::Now().ToUnixTimestamp()));
	const FVector2D ElevationNoiseOffset = FVector2D(RandomStream.FRandRange(-100000.f, 100000.f), RandomStream.FRandRange(-100000.f, 100000.f));
	const FVector2D ResourceNoiseOffset = FVector2D(RandomStream.FRandRange(-100000.f, 100000.f), RandomStream.FRandRange(-100000.f, 100000.f));
	
	// Generate a Path - this is done using a Greedy search through some Poisson Blue Noise
	TMap<FTileCoord, FTileCoord> Path = UPathGenerationBlueprintLibrary::GenerateGreedyPoissonPath(MainPathLength, ElevationNoiseOffset, RandomStream);
	

	TArray<float> NoiseDistribution;
	TMap<FTileCoord, AMapTile*> MapTiles;
	TArray<FTileCoord> PathTiles;
	for (TTuple<FTileCoord, FTileCoord> Elem : Path)
	{
		PathTiles.Add(Elem.Key);
	}
	for (const FTileCoord Coord : FTileCoord::GetTilesInRadius(PathTiles, 10))
	{
		//if (Path.Contains(Coord)) { continue; }
		FVector Location = Coord.ToWorldLocation();
		FRotator Rotation(0.0f, 0.0f, 0.0f);
		
		FActorSpawnParameters SpawnInfo;
	
		// Spawn actual tile BP in world
		AMapTile* Tile = GetWorld()->SpawnActor<AMapTile>(GrassTileBP, Location, Rotation);
		Tile->TileCoord = Coord.Copy();

		float Gradient = (UPathGenerationBlueprintLibrary::GetNodeWeight(Coord, ElevationNoiseOffset, false) + 1.f)/2.f;
		Tile->PathingGradient = Gradient;
		NoiseDistribution.Emplace(Gradient);
		
		Tile->ToggleShowGradients();

		MapTiles.Add(Coord, Tile);
	}

	NoiseDistribution.Sort();
	// Water	10%
	float WaterLimit = NoiseDistribution[static_cast<int>((NoiseDistribution.Num()-1) * 0.1)];
	// Grass 1	50%
	float Grass1Limit = NoiseDistribution[static_cast<int>((NoiseDistribution.Num()-1) * 0.9)];
	// Grass 2	10%
	float Grass2Limit = NoiseDistribution[static_cast<int>((NoiseDistribution.Num()-1) * 0.95)];
	// Grass 3	10%
	float Grass3Limit = NoiseDistribution[static_cast<int>((NoiseDistribution.Num()-1) * 0.97)];
	// Grass 4	10%
	float Grass4Limit = NoiseDistribution[static_cast<int>((NoiseDistribution.Num()-1) * 0.99)];
	// Stone 5	10%
	//float StoneLimit = NoiseDistribution[static_cast<int>(NoiseDistribution.Num() * 1)];
	
	for (TTuple<FTileCoord, AMapTile*> Elem : MapTiles)
	{
		if (PathTiles.Contains(Elem.Key))
		{
			Elem.Value->TileMesh->SetWorldScale3D(FVector(1, 1, 0.75));
			continue;
		}
		Elem.Value->ToggleShowGradients();
		if (Elem.Value->PathingGradient < WaterLimit)
		{
			Elem.Value->TileMesh->SetWorldScale3D(FVector(1, 1, 0.5));
		} else if (Elem.Value->PathingGradient < Grass1Limit)
		{
			Elem.Value->TileMesh->SetWorldScale3D(FVector(1, 1, 1));
		} else if (Elem.Value->PathingGradient < Grass2Limit)
		{
			Elem.Value->TileMesh->SetWorldScale3D(FVector(1, 1, 1.5));
		} else if (Elem.Value->PathingGradient < Grass3Limit)
		{
			Elem.Value->TileMesh->SetWorldScale3D(FVector(1, 1, 2));
		} else if (Elem.Value->PathingGradient < Grass4Limit)
		{
			Elem.Value->TileMesh->SetWorldScale3D(FVector(1, 1, 2.5));
		} else
		{
			Elem.Value->TileMesh->SetWorldScale3D(FVector(1, 1, 3));
		}
	}

	
	// Add nexus building
	ANexusBuilding* NexusBuilding = GetWorld()->SpawnActor<ANexusBuilding>(NexusBuildingBP, FVector(0,0,0), FRotator(0,0,0));

	// Set path start tile coords
	const TArray<FTileCoord> PathStartTileCoords = GetPathStartCoords(Path);

	// Generate tiles - setting tile heights, and tile types

	// Create Map instance
	// Create Map
	UAegisMap* Map = NewObject<UAegisMap>(GetWorld(), AegisMapClass);
	Map->PopulateMapData(MapTiles, Path, PathStartTileCoords, NexusBuilding);

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
