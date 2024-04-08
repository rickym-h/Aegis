// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TileCoordHelperLibrary.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MapTiles/MapTileData.h"
#include "PathGenerationBlueprintLibrary.generated.h"

struct FTileCoord;
/**
 * 
 */
UCLASS()
class AEGIS_API UPathGenerationBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:		
	static TArray<FVector2d> GetBlueNoiseClusters(int GenerationRadius, int PoissonRadius, int SamplesCount, FRandomStream RandomStream);

	static float GetNodeWeight(FTileCoord Tile, FVector2D NoiseOffset, const bool bDistortNoise, const int32 PerlinScale);
	static bool IsPathValid(FTileCoord StartTile, FTileCoord GoalTile, TMap<FTileCoord, FTileCoord> Map);
	// Uses A* to find a path between two points. Graph weights calculated using Perlin noise, making terrain impassable when noise is more/less than the boundary limit, and where there is already a path 
	static TMap<FTileCoord, FTileCoord> AStarPathFind(FTileCoord StartTile, FTileCoord GoalTile, FVector2D PathingNoiseOffset, TMap<FTileCoord, FTileCoord> ExistingPath);
	static TArray<FTileCoord> GetPoissonClusterCoords(int GenerationRadius, int Poisson_Radius, int SamplesCount, FRandomStream RandomStream);
	static TMap<FTileCoord, FTileCoord> GenerateGreedyPoissonPath(const int MainPathLength, const FVector2d ElevationNoiseOffset, FRandomStream RandomStream);
	
	static TMap<FTileCoord, UMapTileData*> GenerateMapTilesData(const TMap<FTileCoord, FTileCoord>& Path, FVector2D ElevationNoiseOffset, FVector2D TreeNoiseOffset, FVector2D StoneNoiseOffset, FRandomStream RandomStream);
};
