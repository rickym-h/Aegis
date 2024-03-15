// Fill out your copyright notice in the Description page of Project Settings.


#include "PathGenerationBlueprintLibrary.h"

#include <algorithm>

#include "HeadMountedDisplayTypes.h"

TArray<FTileCoord> UPathGenerationBlueprintLibrary::GetClustersInRange(const int MapRadius)
{
	TArray<FTileCoord> ClustersQueue;
	TSet<FTileCoord> ClustersInRange;
	if (MapRadius < 5)
	{
		UE_LOG(LogTemp, Error, TEXT("UPathGenerationBlueprintLibrary::GetClustersInRange() - MapRadius is out of range."))
		return ClustersQueue;
	}

	ClustersQueue.Add(FTileCoord());
	ClustersInRange.Add(FTileCoord());
	while (ClustersQueue.Num() > 0)
	{
		// Pop the top off the queue
		const FTileCoord CurrentCoord = ClustersQueue.Pop();
		
		// Get all adjacent coords
		for (FTileCoord AdjacentCoord : GetAdjacentClusters(CurrentCoord))
		{
			// If they are not already in the set, add them to both the set and queue
			if (FTileCoord::HexDistance(FTileCoord(), AdjacentCoord) > MapRadius) { continue; }
			if (ClustersInRange.Contains(AdjacentCoord)) { continue; }
			
			ClustersQueue.Add(AdjacentCoord);
			ClustersInRange.Add(AdjacentCoord);
		}
	}
	return ClustersInRange.Array();
}

TArray<FTileCoord> UPathGenerationBlueprintLibrary::GetAdjacentClusters(const FTileCoord Coord)
{
	TArray<FTileCoord> TileCoordOffsets;
	TileCoordOffsets.Add(FTileCoord(3,0));
	TileCoordOffsets.Add(FTileCoord(0,3));
	TileCoordOffsets.Add(FTileCoord(-3,3));
	TileCoordOffsets.Add(FTileCoord(-3,0));
	TileCoordOffsets.Add(FTileCoord(0,-3));
	TileCoordOffsets.Add(FTileCoord(3,-3));

	TArray<FTileCoord> AdjacentClusterCoords;
	for (FTileCoord TileCoordOffset : TileCoordOffsets)
	{
		AdjacentClusterCoords.Add(Coord+TileCoordOffset);
	}

	return AdjacentClusterCoords;
}

TMap<FTileCoord, FTileCoord> UPathGenerationBlueprintLibrary::GenerateStraightPathBetweenPoints(const FTileCoord Start, const FTileCoord End)
{
	TMap<FTileCoord, FTileCoord> Path;

	FTileCoord Frontier = Start;
	
	while (Frontier != End)
	{
		TArray<FTileCoord> AdjacentTiles = FTileCoord::GetTilesInRadius(Frontier, 1);

		AdjacentTiles.Sort([End](FTileCoord Left, FTileCoord Right)
		{
			return (FTileCoord::HexDistance(End, Left) < FTileCoord::HexDistance(End, Right));
		});

		Path.Add(Frontier, AdjacentTiles[0]);
		Frontier = AdjacentTiles[0];
	}
	
	return Path;
}

TMap<FTileCoord, FTileCoord> UPathGenerationBlueprintLibrary::GetFullPathFromClusters(TMap<FTileCoord, FTileCoord> PathClusters)
{
	TMap<FTileCoord, FTileCoord> FullPath;

	for (TTuple<FTileCoord, FTileCoord> Entry : PathClusters)
	{
		 FullPath.Append(GenerateStraightPathBetweenPoints(Entry.Key, Entry.Value));
	}

	return FullPath;
}

int UPathGenerationBlueprintLibrary::WeightedRandomIndex(TArray<FTileCoord> Items, TArray<float> Weights)
{
	float TotalWeight = 0;
	for (const float Weight : Weights)
	{
		TotalWeight += Weight;
	}
	TArray<float> AdjustedWeights;
	for (const float Weight : Weights)
	{
		AdjustedWeights.Add(Weight/TotalWeight);
	}

	const float Random = FMath::FRand();
	float CumulativeSum = 0;
	
	for (int i = 0; i < Items.Num(); i++)
	{
		CumulativeSum += AdjustedWeights[i];
		if (CumulativeSum > Random)
		{
			return i;
		}
	}
	return Items.Num()-1;
}

TArray<FTileCoord> UPathGenerationBlueprintLibrary::GetAdjacentClustersRandom(const FTileCoord Coord)
{
	TArray<FTileCoord> AdjacentClusters = GetAdjacentClusters(Coord);
	TArray<float> Weights;
	if (Coord == FTileCoord())
	{
		Weights.Init(1, AdjacentClusters.Num());
	} else
	{
		for (FTileCoord AdjacentCluster : AdjacentClusters)
		{
			// TODO Add weight depending on angle from centre
			Weights.Add(1.f);
		}
	}

	const int Count = AdjacentClusters.Num();

	TArray<FTileCoord> RandomlySortedAdjacentClusters;
	while (RandomlySortedAdjacentClusters.Num() != Count)
	{
		const int Index = WeightedRandomIndex(AdjacentClusters, Weights);
		RandomlySortedAdjacentClusters.Add(AdjacentClusters[Index]);
		
		AdjacentClusters.RemoveAt(Index);
		Weights.RemoveAt(Index);
	}
	return RandomlySortedAdjacentClusters;
}

bool UPathGenerationBlueprintLibrary::PathMapIsValid(const TMap<FTileCoord, FTileCoord> Map, const int PathLengthInClusters, const int PathsCount, const int BranchesCount)
{
	int PathsCountCount = 0;
	int PathLengthInClustersCount = 0;
	
	// Iterate over every path connection
	for (TTuple<FTileCoord, FTileCoord> Entry : Map)
	{
		// Count number of times the path goes into the centre tile
		if (Entry.Value == FTileCoord())
		{
			PathsCountCount++;
		}

		// Count number of paths total
		PathLengthInClustersCount++;
	}

	if (PathsCountCount-1 == PathsCount && PathLengthInClustersCount-1 == PathLengthInClusters)
	{
		return true;
	}
	return false;
}

FTileCoord UPathGenerationBlueprintLibrary::GetClosestEndToCentre(const TMap<FTileCoord, FTileCoord> Map)
{
	if (Map.Num() == 1)  { return FTileCoord(); }
	FTileCoord ClosestSoFar;
	int ClosestCountSoFar = INT_MAX;

	TSet<FTileCoord> NodesPointedTo;
	for (const TTuple<FTileCoord, FTileCoord> Entry : Map)
	{
		NodesPointedTo.Add(Entry.Value);
	}
	TMap<FTileCoord, FTileCoord> NodesToCheck;
	for (const TTuple<FTileCoord, FTileCoord> Entry : Map)
	{
		if (!NodesPointedTo.Contains(Entry.Key))
		{
			NodesToCheck.Add(Entry.Key, Entry.Value);
		}
	}

	for (const TTuple<FTileCoord, FTileCoord> Entry : NodesToCheck)
	{
		// If there exists another coords that points to this coord, skip it
		int DistanceToCentre = 0;
		FTileCoord PointingTo = Entry.Value;
		while (PointingTo != FTileCoord())
		{
			DistanceToCentre++;
			PointingTo = Map[PointingTo];
		}

		if (DistanceToCentre < ClosestCountSoFar)
		{
			ClosestCountSoFar = DistanceToCentre;
			ClosestSoFar = Entry.Key;
		}
	}

	return ClosestSoFar;
}

TMap<FTileCoord, FTileCoord> UPathGenerationBlueprintLibrary::DeepCopyPath(const TMap<FTileCoord, FTileCoord> Map)
{
	TMap<FTileCoord, FTileCoord> CopiedPath;

	for (TTuple<FTileCoord, FTileCoord> Entry : Map)
	{
		CopiedPath.Add(Entry.Key.Copy(), Entry.Value.Copy());
	}

	return CopiedPath;
}
