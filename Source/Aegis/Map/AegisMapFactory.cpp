// Fill out your copyright notice in the Description page of Project Settings.


#include "AegisMapFactory.h"

#include "MapTile.h"
#include "Aegis/Buildings/NexusBuilding.h"

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

UAegisMap* UAegisMapFactory::GenerateTestMap() const
{
	UE_LOG(LogTemp, Warning, TEXT("GENERATING TEST MAP"))

	// The number of steps needed to reach farthest tile from the centre(e.g. 1->1 Tile, 2->7 Tiles)
	constexpr int MapRadiusInTiles = 9;

	
	if (!GrassTileBP)
	{
		UE_LOG(LogTemp, Error, TEXT("GrassTileBP not valid"))
		return NewObject<UAegisMap>();
	}


	// Generate a path
	// Set the path coords
	TMap<FTileCoord, FTileCoord> PathRoute;
	for (int Q = MapRadiusInTiles; Q > 0; Q--)
	{
		const FTileCoord FromCoord = FTileCoord(Q,0);
		const FTileCoord ToCoord = FTileCoord(Q-1,0);
		PathRoute.Add(FromCoord, ToCoord);
	}
	PathRoute.Add(FTileCoord(), FTileCoord());

	PathRoute.Add(FTileCoord(7,0), FTileCoord(6,1));
	PathRoute.Add(FTileCoord(6,1), FTileCoord(6,0));
	
	TArray<FTileCoord> PathStartTileCoords;
	PathStartTileCoords.Add(FTileCoord(MapRadiusInTiles, 0));
	
	

	// Random offset to use for perlin noise generation (to be seemingly unique every time) as FMath::PerlinNoise2D is not seeded
	const FVector2D RandomNoiseOffset = FVector2D(FMath::FRandRange(-100000.f, 1000000.f));

	// Generate Tiles for map (looping R and Q constraints from RedBlobGames)
	TMap<FTileCoord, AMapTile*> MapTiles;
	for (int Q = -MapRadiusInTiles; Q <= MapRadiusInTiles; Q++)
	{
		for (int R = FMath::Max(-MapRadiusInTiles, -Q-MapRadiusInTiles); R <= FMath::Min(MapRadiusInTiles, -Q+MapRadiusInTiles); R++)
		{
			const FTileCoord ThisTileCoord = FTileCoord(Q, R);
			//if (FTileCoord::HexDistance(CentreCoord, ThisTileCoord) >= MapRadiusInTiles) { continue; }
			
			// World location of tile BP
			FVector Location = (R * OffsetR) + (Q * OffsetQ);
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

			if (ThisTileCoord.S >= 0)
			{
				//Tile->ToggleShowGradients();
			}
		}
	}


	// Add buildings
	ANexusBuilding* NexusBuilding = GetWorld()->SpawnActor<ANexusBuilding>(NexusBuildingBP, FVector(0,0,0), FRotator(0,0,0));

	UAegisMap* Map = NewObject<UAegisMap>();
	Map->PopulateMapData(MapTiles, PathRoute, PathStartTileCoords, NexusBuilding);
	
	return Map;
}
