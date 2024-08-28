// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>

#include "CoreMinimal.h"
#include "AegisMap.h"
#include "AegisMapFactory.generated.h"


struct Point {
	float x, y;

	Point() : x{0}, y{0} {}
	Point(float _x, float _y) : x{_x}, y{_y} {}

	bool operator==(const Point& other) const
	{
		return (other.x == x && other.y == y);
	}

	bool operator!=(const Point& other) const { return !operator==(other); }
};

struct Edge {
	Point p0, p1;

	Edge(Point const& _p0, Point const& _p1) : p0{_p0}, p1{_p1} {}

	bool operator==(const Edge& other) const
	{
		return ((other.p0 == p0 && other.p1 == p1) ||
				(other.p0 == p1 && other.p1 == p0));
	}
};

FORCEINLINE uint32 GetTypeHash(const Point& Point)
{
	return FCrc::MemCrc32(&Point, sizeof(Point));
}

struct Circle {
	float x, y, radius;
	Circle() = default;
};

struct Triangle {
	Point p0, p1, p2;
	Edge e0, e1, e2;
	Circle circle;

	Triangle(const Point& _p0, const Point& _p1, const Point& _p2)
		: p0{_p0},
		  p1{_p1},
		  p2{_p2},
		  e0{_p0, _p1},
		  e1{_p1, _p2},
		  e2{_p0, _p2},
		  circle{}
	{
		const auto ax = p1.x - p0.x;
		const auto ay = p1.y - p0.y;
		const auto bx = p2.x - p0.x;
		const auto by = p2.y - p0.y;

		const auto m = p1.x * p1.x - p0.x * p0.x + p1.y * p1.y - p0.y * p0.y;
		const auto u = p2.x * p2.x - p0.x * p0.x + p2.y * p2.y - p0.y * p0.y;
		const auto s = 1. / (2. * (ax * by - ay * bx));

		circle.x = ((p2.y - p0.y) * m + (p0.y - p1.y) * u) * s;
		circle.y = ((p0.x - p2.x) * m + (p1.x - p0.x) * u) * s;

		const auto dx = p0.x - circle.x;
		const auto dy = p0.y - circle.y;
		circle.radius = dx * dx + dy * dy;
	}
};

struct Delaunay {
	std::vector<Triangle> triangles;
	std::vector<Edge> edges;
};

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


	UFUNCTION()
	UAegisGameMap* GenerateGameMap(const int PathLengthInNodes) const;
	UFUNCTION()
	UAegisWorldMap* GenerateWorldMap() const;

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
	static TMap<FTileCoord, TSet<FTileCoord>> GenerateDelaunayTriangulation(const TArray<FTileCoord>& PoissonNodeCoords);
	static TArray<FTileCoord> SelectRandomPathThroughNodeGraph(const int32 PathLengthInNodes, const TMap<FTileCoord, TSet<FTileCoord>>& PoissonNodeGraph, FRandomStream RandomStream);
	static TMap<FTileCoord, FTileCoord> GenerateAStarPathThroughNodes(const TArray<FTileCoord>& PathNodes, FVector2D PathingNoiseOffset);
	static bool AStarPathFind(const FTileCoord StartTile, const FTileCoord GoalTile, FVector2D PathingNoiseOffset, const TMap<FTileCoord, FTileCoord>& ExistingPath, TMap<FTileCoord, FTileCoord>& OutputPath, float WeightExponent);
	static TMap<FTileCoord, UMapTileData*> GenerateMapTilesDataAroundPath(const TMap<FTileCoord, FTileCoord>& Path, FVector2D ElevationNoiseOffset, FVector2D TreeNoiseOffset, FVector2D StoneNoiseOffset);
	
	static TArray<FTileCoord> GetPathStartCoords(TMap<FTileCoord, FTileCoord> PathRoute);
	static bool IsPathValid(const FTileCoord StartTile, const FTileCoord GoalTile, const TMap<FTileCoord, FTileCoord>& Path);
	static float GetNodeWeight(const FTileCoord Tile, const FVector2D NoiseOffset, const bool bDistortNoise, const int32 PerlinScale);
};
