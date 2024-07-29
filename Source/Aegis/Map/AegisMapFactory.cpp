// Fill out your copyright notice in the Description page of Project Settings.


#include "AegisMapFactory.h"

#include "MapTile.h"
#include "PathGenerationBlueprintLibrary.h"
#include "Aegis/Structures/NexusBuilding/NexusBuilding.h"

void UAegisMapFactory::PostInitProperties()
{
	UObject::PostInitProperties();

	if (!GetWorld()) { return; }

	TileHeight = 2 * TileRadius;
	TileWidth = FMath::Sqrt(3.f) * TileRadius;

	VerticalSpacing = 1.5 * TileRadius;
	HorizontalSpacing = TileWidth;

	OffsetQ = FVector(0, HorizontalSpacing, 0);
	OffsetR = FVector(VerticalSpacing, HorizontalSpacing / 2, 0);
}

UAegisMap* UAegisMapFactory::GenerateMapWithNoise(const int PathNodeLength) const
{
	const FRandomStream RandomStream = FRandomStream(static_cast<int32>(FDateTime::Now().ToUnixTimestamp()));
	const FVector2D ElevationNoiseOffset = FVector2D(RandomStream.FRandRange(-100000.f, 100000.f), RandomStream.FRandRange(-100000.f, 100000.f));
	const FVector2D PathingNoiseOffset = FVector2D(RandomStream.FRandRange(-100000.f, 100000.f), RandomStream.FRandRange(-100000.f, 100000.f));
	const FVector2D TreeNoiseOffset = FVector2D(RandomStream.FRandRange(-100000.f, 100000.f), RandomStream.FRandRange(-100000.f, 100000.f));
	const FVector2D StoneNoiseOffset = FVector2D(RandomStream.FRandRange(-100000.f, 100000.f), RandomStream.FRandRange(-100000.f, 100000.f));


	// Generate a Path - this is done using a Greedy search through some Poisson Blue Noise
	constexpr int POISSON_RADIUS = 8;
	constexpr int GENERATION_RADIUS = 100;
	const TArray<FTileCoord> PoissonNodeCoords = UPathGenerationBlueprintLibrary::GetPoissonClusterCoords(GENERATION_RADIUS, POISSON_RADIUS, 5000, RandomStream);

	const TMap<FTileCoord, TSet<FTileCoord>> PoissonNodeGraph = UPathGenerationBlueprintLibrary::GeneratePoissonNodeGraph(PoissonNodeCoords, POISSON_RADIUS, GENERATION_RADIUS, GetWorld());

	// Draw lines between each connected node
	// for (TPair<FTileCoord, TSet<FTileCoord>> Pair : PoissonNodeGraph)
	// {
	// 	for (FTileCoord AdjacentNode : Pair.Value)
	// 	{
	// 		FVector Start = Pair.Key.ToWorldLocation() + FVector(0, 0, 400); // Origin
	// 		FVector End = AdjacentNode.ToWorldLocation() + FVector(0, 0, 400); // Endpoint
	//
	// 		FColor Color = FColor::Red; // Line color
	//
	// 		// Draw the debug line
	// 		DrawDebugLine(GetWorld(), Start, End, Color, true, -1, 0, 5);
	// 	}
	// }
	
	const TMap<FTileCoord, FTileCoord> Path = UPathGenerationBlueprintLibrary::GenerateGreedyPoissonPath(PathNodeLength, PoissonNodeCoords, PathingNoiseOffset, RandomStream);

	const TMap<FTileCoord, UMapTileData*> MapTilesData = UPathGenerationBlueprintLibrary::GenerateMapTilesData(
		Path, ElevationNoiseOffset, TreeNoiseOffset, StoneNoiseOffset, RandomStream);

	// Add nexus building
	ANexusBuilding* NexusBuilding = GetWorld()->SpawnActor<ANexusBuilding>(NexusBuildingBP, FVector(0, 0, 0), FRotator(0, 0, 0));

	// Set path start tile coords
	const TArray<FTileCoord> PathStartTileCoords = GetPathStartCoords(Path);

	// Create Map instance
	UAegisMap* Map = NewObject<UAegisMap>(GetWorld(), AegisMapClass);
	Map->PopulateMapData(MapTilesData, Path, PathStartTileCoords, NexusBuilding);

	// Set TilesToEnd of every tile
	// Post Map creation settings
	for (AMapTile* Tile : Map->GetTiles())
	{
		Tile->TilesToEnd = Map->GetNumOfTilesToEnd(Tile->TileCoord);
	}

	UE_LOG(LogTemp, Warning, TEXT("UAegisMapFactory::GenerateMapWithNoise - Node count: %i"), PoissonNodeCoords.Num())

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
