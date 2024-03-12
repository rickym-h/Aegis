// Fill out your copyright notice in the Description page of Project Settings.


#include "AegisMapFactory.h"

#include "MapTile.h"
#include "Aegis/Structures/NexusBuilding.h"

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

UAegisMap* UAegisMapFactory::GenerateMap(const int MapClusterRadius, const int PathClusterLength) const
{
	// Check the input parameters are valid
	if (MapClusterRadius < 1 || MapClusterRadius > 20)
	{
		UE_LOG(LogTemp, Error, TEXT("UAegisMapFactory::GenerateMap() - MapClusterRadius Out of range."))
	}
	if (PathClusterLength < MapClusterRadius)
	{
		UE_LOG(LogTemp, Error, TEXT("UAegisMapFactory::GenerateMap() - PathClusterLength too short."))
	}
	// Check factory classes are valid
	if (!GrassTileBP)
	{
		UE_LOG(LogTemp, Error, TEXT("GrassTileBP not valid"))
		return NewObject<UAegisMap>();
	}

	// Generate Map Data

	const TMap<FTileCoord, FTileCoord> PathRoute = GeneratePath(MapClusterRadius, PathClusterLength);

	const TMap<FTileCoord, AMapTile*> MapTiles = GenerateMapTiles(MapClusterRadius, PathRoute);

	const TArray<FTileCoord> PathStartTileCoords = GetPathStartCoords(PathRoute);
	
	ANexusBuilding* NexusBuilding = GetWorld()->SpawnActor<ANexusBuilding>(NexusBuildingBP, FVector(0,0,0), FRotator(0,0,0));

	// Create Map
	UAegisMap* Map = NewObject<UAegisMap>(GetWorld(), AegisMapClass);
	Map->PopulateMapData(MapTiles, PathRoute, PathStartTileCoords, NexusBuilding);

	// Post Map creation settings
	for (AMapTile* Tile : Map->GetTiles())
	{
		Tile->TilesToEnd = Map->GetNumOfTilesToEnd(Tile->TileCoord);
	}

	return Map;
}



TMap<FTileCoord, AMapTile*> UAegisMapFactory::GenerateMapTiles(const int MapClusterRadius, const TMap<FTileCoord, FTileCoord>& PathRoute) const
{
	const int MapRadiusInTiles = MapClusterRadius * 3;
	// Random offset to use for perlin noise generation (to be seemingly unique every time) as FMath::PerlinNoise2D is not seeded
	const FVector2D RandomNoiseOffset = FVector2D(FMath::FRandRange(-100000.f, 1000000.f));
	
	TMap<FTileCoord, AMapTile*> MapTiles;
	for (const FTileCoord ThisTileCoord : FTileCoord::GetTilesInRadius(FTileCoord(), MapRadiusInTiles))
	{
		int Q = ThisTileCoord.Q;
		int R = ThisTileCoord.R;
		
		// World location of tile BP
		FVector Location = ThisTileCoord.ToWorldLocation();
		FRotator Rotation(0.0f, 0.0f, 0.0f);
			
		FActorSpawnParameters SpawnInfo;

		// TODO decide if it should be a GrassTileBP or a PathTile
		// (for now it is just visualised by toggling debug gradients at end of tile creation)

		// Spawn actual tile BP in world
		AMapTile* Tile = GetWorld()->SpawnActor<AMapTile>(GrassTileBP, Location, Rotation);

		FVector2D NoiseSampleLoc = FVector2D(Location.X/100, Location.Y/100) + RandomNoiseOffset;
		float NoiseValue = FMath::PerlinNoise2D(NoiseSampleLoc);
		NoiseValue = (NoiseValue+1)/2; // Map from -1-1 to 0-1

		// Curve Temp using S curve
		NoiseValue = 1/(1+FMath::Pow(NoiseValue/(1-NoiseValue), -3));
		// Square to bring lower noise values lower (So pathing might opt for longer routes with less noise rather than going through higher noise tiles)
		NoiseValue = FMath::Pow(NoiseValue, 2);

		// Clamp the noise to between 0 and 1 in case scaling has placed noise out of bounds
		NoiseValue = FMath::Min(NoiseValue, 1.f);
		NoiseValue = FMath::Max(NoiseValue, 0.f);
			
		Tile->TileCoord = ThisTileCoord.Copy();
		Tile->PathingGradient = NoiseValue;

		if (PathRoute.Contains(ThisTileCoord))
		{
			Tile->ToggleShowGradients();
		}
			
		//Map->AddTileToMap(ThisTileCoord, Tile);
		MapTiles.Add(ThisTileCoord, Tile);
	}

	return MapTiles;
}

TMap<FTileCoord, FTileCoord> UAegisMapFactory::GeneratePath(int MapClusterRadius, int PathClusterLength) const
{
	const int MapRadiusInTiles = MapClusterRadius * 3;
	
	TMap<FTileCoord, FTileCoord> PathRoute;
	for (int Q = MapRadiusInTiles; Q > 0; Q--)
	{
		const FTileCoord FromCoord = FTileCoord(Q,0);
		const FTileCoord ToCoord = FTileCoord(Q-1,0);
		PathRoute.Add(FromCoord, ToCoord);
	}
	PathRoute.Add(FTileCoord(), FTileCoord());

	return PathRoute;
}

TArray<FTileCoord> UAegisMapFactory::GetPathStartCoords(TMap<FTileCoord, FTileCoord> PathRoute) const
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
