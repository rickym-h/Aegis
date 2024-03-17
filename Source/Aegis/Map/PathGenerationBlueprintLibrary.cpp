// Fill out your copyright notice in the Description page of Project Settings.


#include "PathGenerationBlueprintLibrary.h"

TArray<FTileCoord> UPathGenerationBlueprintLibrary::GetClustersInRange(const FTileCoord Origin, const int MapRadius)
{
	TArray<FTileCoord> ClustersQueue;
	TSet<FTileCoord> ClustersInRange;
	if (MapRadius < 1)
	{
		UE_LOG(LogTemp, Error, TEXT("UPathGenerationBlueprintLibrary::GetClustersInRange() - MapRadius is out of range."))
		return ClustersQueue;
	}

	ClustersQueue.Add(Origin);
	ClustersInRange.Add(Origin);
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
	constexpr int ClusterSpacing = 4;
	
	TArray<FTileCoord> TileCoordOffsets;
	TileCoordOffsets.Add(FTileCoord(ClusterSpacing,0));
	TileCoordOffsets.Add(FTileCoord(0,ClusterSpacing));
	TileCoordOffsets.Add(FTileCoord(-ClusterSpacing,ClusterSpacing));
	TileCoordOffsets.Add(FTileCoord(-ClusterSpacing,0));
	TileCoordOffsets.Add(FTileCoord(0,-ClusterSpacing));
	TileCoordOffsets.Add(FTileCoord(ClusterSpacing,-ClusterSpacing));

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

	for (const TTuple<FTileCoord, FTileCoord> Entry : PathClusters)
	{
		 FullPath.Append(GenerateStraightPathBetweenPoints(Entry.Key, Entry.Value));
	}

	return FullPath;
}

int UPathGenerationBlueprintLibrary::WeightedRandomIndex(const TArray<FTileCoord>& Items, TArray<float> Weights)
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

TArray<FTileCoord> UPathGenerationBlueprintLibrary::GetAllPathEnds(const TMap<FTileCoord, FTileCoord> Map)
{
	TSet<FTileCoord> NodesPointedTo;
	for (const TTuple<FTileCoord, FTileCoord> Entry : Map)
	{
		NodesPointedTo.Add(Entry.Value);
	}
	TArray<FTileCoord> PathEnds;
	for (const TTuple<FTileCoord, FTileCoord> Entry : Map)
	{
		if (!NodesPointedTo.Contains(Entry.Key))
		{
			PathEnds.Add(Entry.Key);
		}
	}
	return PathEnds;
}

FTileCoord UPathGenerationBlueprintLibrary::GetClosestEndToCentre(const TMap<FTileCoord, FTileCoord> Map)
{
	if (Map.Num() == 1)  { return FTileCoord(); }
	FTileCoord ClosestSoFar = FTileCoord();
	int ClosestCountSoFar = INT_MAX;

	TArray<FTileCoord> PathEnds = GetAllPathEnds(Map);

	for (const FTileCoord End : PathEnds)
	{
		// If there exists another coords that points to this coord, skip it
		int DistanceToCentre = 0;
		FTileCoord PointingTo = Map[End];
		while (PointingTo != FTileCoord())
		{
			DistanceToCentre++;
			PointingTo = Map[PointingTo];
		}

		if (DistanceToCentre < ClosestCountSoFar)
		{
			ClosestCountSoFar = DistanceToCentre;
			ClosestSoFar = End;
		}
	}

	return ClosestSoFar;
}

bool UPathGenerationBlueprintLibrary::IsClusterBranchable(const FTileCoord Cluster, const TMap<FTileCoord, FTileCoord>& Map)
{
	for (const FTileCoord AdjacentTile : GetAdjacentClusters(Cluster))
	{
		if (!Map.Contains(AdjacentTile))
		{
			return true;
		}
	}
	return false;
}

int UPathGenerationBlueprintLibrary::GetNumOfBranchesFromPath(FTileCoord PathEnd, const TMap<FTileCoord, FTileCoord>& Map)
{
	// CoordsInPath should exclude the FTileCoord()
	TArray<FTileCoord> CoordsInPath;
	FTileCoord NextCoord = PathEnd;
	while (NextCoord != FTileCoord())
	{
		CoordsInPath.Add(NextCoord);
		NextCoord = Map[NextCoord];
	}

	int Count = 0;
	for (TTuple<FTileCoord, FTileCoord> Entry : Map)
	{
		if (CoordsInPath.Contains(Entry.Value) && !CoordsInPath.Contains(Entry.Key))
		{
			Count++;
		}
	}

	return Count;
}

FTileCoord UPathGenerationBlueprintLibrary::GetClusterToBranchFrom(const TMap<FTileCoord, FTileCoord> Map)
{
	TArray<FTileCoord> EndClusters = GetAllPathEnds(Map);

	// TODO sort by branches so far
	// TODO if multiple, select the one that is furthest from the centre
    
	
	EndClusters.Sort([Map](const FTileCoord Left, const FTileCoord Right)
	{
		return GetNumOfBranchesFromPath(Left, Map) < GetNumOfBranchesFromPath(Right, Map);
	});

	for (FTileCoord EndCluster : EndClusters)
	{
		FTileCoord NodeToCheck = Map[EndCluster];
		while (NodeToCheck != FTileCoord())
		{
			if (IsClusterBranchable(NodeToCheck, Map))
			{
				return NodeToCheck;
			}
			NodeToCheck = Map[NodeToCheck];
		}
	}
	
	return FTileCoord();
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
