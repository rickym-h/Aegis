// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AegisMap.h"
#include "AegisMapFactory.generated.h"

class UAegisGameMap;
class UAegisWorldMap;
class ANexusBuilding;
class AMapTile;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class AEGIS_API UAegisMapFactory : public UObject
{
	GENERATED_BODY()

public:
	virtual void PostInitProperties() override;

	static TArray<FTileCoord> GetPathStartCoords(TMap<FTileCoord, FTileCoord> PathRoute);

	UFUNCTION()
	UAegisGameMap* GenerateGameMap(const int PathLengthInNodes) const;
	UFUNCTION()
	UAegisGameMap* GenerateWorldMap() const;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Map")
	TSubclassOf<UAegisGameMap> AegisGameMapClass;
	UPROPERTY(EditDefaultsOnly, Category="Map")
	TSubclassOf<UAegisWorldMap> WorldMapClass;
	
	UPROPERTY(EditDefaultsOnly, Category="Tiles")
	TSubclassOf<ANexusBuilding> NexusBuildingBP;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Tiles")
	int TileRadius = 100;

	UPROPERTY(BlueprintReadOnly, Category="Tiles")
	float TileHeight;
	UPROPERTY(BlueprintReadOnly, Category="Tiles")
	float TileWidth;
	UPROPERTY(BlueprintReadOnly, Category="Tiles")
	float VerticalSpacing;
	UPROPERTY(BlueprintReadOnly, Category="Tiles")
	float HorizontalSpacing;

	UPROPERTY(BlueprintReadOnly, Category="Tiles")
	FVector OffsetQ;
	UPROPERTY(BlueprintReadOnly, Category="Tiles")
	FVector OffsetR;

	static FVector2D GetRandomNoiseOffset(const FRandomStream& RandomStream);

	static TArray<FTileCoord> GeneratePoissonNodeCoords(const int32 PoissonDiskRadiusInMetres, const int32 GenerationRadiusInMetres, const int32 MaxSampleCount, const FRandomStream RandomStream);
	static TArray<FVector2D> GetPoissonDiskClusters(const int32 PoissonDiskRadiusInMetres, const int32 GenerationRadiusInMetres, const int32 MaxSampleCount, const FRandomStream RandomStream);
	static TMap<FTileCoord, TSet<FTileCoord>> GeneratePseudoDelaunayTriangulation(const TArray<FTileCoord>& PoissonNodeCoords, const int PoissonRadius);
	static TArray<FTileCoord> SelectRandomPathThroughNodeGraph(const int32 PathLengthInNodes, const TMap<FTileCoord, TSet<FTileCoord>>& PoissonNodeGraph, FRandomStream RandomStream);
	static TMap<FTileCoord, FTileCoord> GenerateAStarPathThroughNodes(const TArray<FTileCoord>& PathNodes, FVector2D PathingNoiseOffset);
	static TMap<FTileCoord, FTileCoord> AStarPathFind(const FTileCoord StartTile, const FTileCoord GoalTile, FVector2D PathingNoiseOffset, const TMap<FTileCoord, FTileCoord>& ExistingPath);
	static TMap<FTileCoord, UMapTileData*> GenerateMapTilesDataAroundPath(const TMap<FTileCoord, FTileCoord>& Path, FVector2D ElevationNoiseOffset, FVector2D TreeNoiseOffset, FVector2D StoneNoiseOffset);
	
	static bool IsPathValid(const FTileCoord StartTile, const FTileCoord GoalTile, const TMap<FTileCoord, FTileCoord>& Path);
	static float GetNodeWeight(const FTileCoord Tile, const FVector2D NoiseOffset, const bool bDistortNoise, const int32 PerlinScale);
};
