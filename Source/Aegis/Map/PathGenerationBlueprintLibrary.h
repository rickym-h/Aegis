// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TileCoordHelperLibrary.h"
#include "Kismet/BlueprintFunctionLibrary.h"
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
	UFUNCTION(BlueprintCallable, Category="Path Generation")
	static TArray<FTileCoord> GetClustersInRange(const FTileCoord Origin, const int MapRadius);
	
	UFUNCTION(BlueprintCallable, Category="Path Generation")
	static TArray<FTileCoord> GetAdjacentClusters(const FTileCoord Coord);

	static TMap<FTileCoord, FTileCoord> GenerateStraightPathBetweenPoints(FTileCoord Start, FTileCoord End);

	UFUNCTION(BlueprintCallable, Category="Path Generation")
	static TMap<FTileCoord, FTileCoord> GetFullPathFromClusters(TMap<FTileCoord, FTileCoord> PathClusters);
	
	static int WeightedRandomIndex(const TArray<FTileCoord>& Items, TArray<float> Weights);
	
	UFUNCTION(BlueprintCallable, Category="Path Generation")
	static TArray<FTileCoord> GetAdjacentClustersRandom(const FTileCoord Coord);

	UFUNCTION(BlueprintCallable, Category="Path Generation")
	static bool PathMapIsValid(const TMap<FTileCoord, FTileCoord> Map, const int PathLengthInClusters, const int PathsCount, const int BranchesCount);
	
	UFUNCTION(BlueprintCallable, Category="Path Generation")
	static TArray<FTileCoord> GetAllPathEnds(const TMap<FTileCoord, FTileCoord> Map);

	UFUNCTION(BlueprintCallable, Category="Path Generation")
	static FTileCoord GetClosestEndToCentre(const TMap<FTileCoord, FTileCoord> Map);
	
	static bool IsClusterBranchable(FTileCoord TileCoord, const TMap<FTileCoord, FTileCoord>& Map);

	static int GetNumOfBranchesFromPath(FTileCoord PathEnd, const TMap<FTileCoord, FTileCoord>& Map);

	UFUNCTION(BlueprintCallable, Category="Path Generation")
	static FTileCoord GetClusterToBranchFrom(const TMap<FTileCoord, FTileCoord> Map);
	
	UFUNCTION(BlueprintCallable, Category="Path Generation")
	static TMap<FTileCoord, FTileCoord> DeepCopyPath(const TMap<FTileCoord, FTileCoord> Map);
	
	static TArray<FVector2d> GetBlueNoiseClusters(int GenerationRadius, int PoissonRadius, int SamplesCount, FRandomStream RandomStream);

	static float GetNodeWeight(FTileCoord Tile, FVector2D NoiseOffset, const bool bSmoothForPathing);
	static bool IsPathValid(FTileCoord StartTile, FTileCoord GoalTile, TMap<FTileCoord, FTileCoord> Map);
	// Uses A* to find a path between two points. Graph weights calculated using Perlin noise, making terrain impassable when noise is more/less than the boundary limit, and where there is already a path 
	static TMap<FTileCoord, FTileCoord> AStarPathFind(FTileCoord StartTile, FTileCoord GoalTile, FVector2D NoiseOffset, TMap<FTileCoord, FTileCoord> ExistingPath);
	static TArray<FTileCoord> GetPoissonClusterCoords(int GenerationRadius, int Poisson_Radius, int SamplesCount, FRandomStream RandomStream);
	static TMap<FTileCoord, FTileCoord> GenerateGreedyPoissonPath(const int MainPathLength, const FVector2d ElevationNoiseOffset, FRandomStream RandomStream);
};
