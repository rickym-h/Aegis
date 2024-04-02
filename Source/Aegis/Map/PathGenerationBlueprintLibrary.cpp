// Fill out your copyright notice in the Description page of Project Settings.


#include "PathGenerationBlueprintLibrary.h"

#include "Aegis/Utilities/TPriorityQueue.h"
#include "Kismet/KismetArrayLibrary.h"

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

TArray<FVector2d> UPathGenerationBlueprintLibrary::GetBlueNoiseClusters(const int GenerationRadius, const int PoissonRadius, const int SamplesCount)
{
	// ReSharper disable once CppTooWideScope
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	constexpr int MAX_TRIALS = 30;

	// Shouldn't *really* use FTileCoord, but we basically just want to be able to compare pairs of ints, and FTileCoord has that functionality,
	// We just have to map X->Q, Y->R, and ignore S
	TMap<FTileCoord, FVector2d> Grid;
	TArray<FVector2d> List;
	const float Bin = static_cast<float>(PoissonRadius) / FMath::Sqrt(2.f);

	// Place the initial point in the Grid and List
	constexpr float X = 0;
	constexpr float Y = 0;
	Grid.Add(FTileCoord(static_cast<int>(X / Bin), static_cast<int>(Y / Bin)), FVector2d(X, Y));
	List.Add(FVector2d(X, Y));

	// Loop until List is empty
	while (List.Num() > 0)
	{
		// Pick a random point from List
		const int K = FMath::RandRange(0, List.Num()-1);
		const float X0 = List[K].X;
		const float Y0 = List[K].Y;

		// Trials to place the next point
        bool FoundTrialPoint =  false;
		for (int Trial = 0; Trial < MAX_TRIALS; Trial++)
		{
			// Generate a trial point int he annulus r->2*r around x0,y0
			const float R = FMath::FRandRange(static_cast<float>(PoissonRadius), static_cast<float>(2*PoissonRadius));
			const float Phi = FMath::FRandRange(0.f, static_cast<float>(2*PI));
			const float TrialX = X0 + R * FMath::Cos(Phi);
			const float TrialY = Y0 + R * FMath::Sin(Phi);

			// Reject point if it is outside the GenerationRadius
			if (TrialX < -GenerationRadius || TrialX > GenerationRadius || TrialY < -GenerationRadius || TrialY > GenerationRadius)
			{
				continue;
			}

			// Find the Grid cell of the trial point
			const int I = static_cast<int>(TrialX / Bin);
			const int J = static_cast<int>(TrialY / Bin);

			// Iterate over neighbours, looking for conflicts with occupied cells
			bool Conflict = false;
			for (int u = -1; u <= 1; u++)
			{
				for (int v = -1; v <= 1; v++)
				{
					if (Grid.Contains(FTileCoord(I + u, J + v)))
					{
						Conflict = true;
						break;
					}
				}
			}

			// If no Conflict, accept trial point into List, and stop trials
			if (!Conflict)
			{
				Grid.Add(FTileCoord(I, J), FVector2d(TrialX, TrialY));
				List.Add(FVector2d(TrialX, TrialY));
				FoundTrialPoint = true;
				break;
			}
		}

		// If gone through all trial points without success, remove initial point from List
		if (!FoundTrialPoint)
		{
			List.RemoveAt(K);
		}

		// Stop if enough points have been generated
		if (Grid.Num() >= SamplesCount)
		{
			break;
		}
	}

	TArray<FVector2d> OutputLocations;
	for (TTuple<FTileCoord, FVector2d> Element : Grid)
	{
		OutputLocations.Add(Element.Value);
	}
	return OutputLocations;	
}

float UPathGenerationBlueprintLibrary::GetNodeWeight(const FTileCoord Tile, const FVector2D NoiseOffset, const bool bSmoothForPathing)
{
	int PerlinScale = 1000;
	if (bSmoothForPathing)
	{
		PerlinScale = 500;
	}
	const FVector Location = Tile.ToWorldLocation();
	const FVector2D NoiseSampleLoc = FVector2D(Location.X/PerlinScale, Location.Y/PerlinScale) + NoiseOffset;
	const float x = (FMath::PerlinNoise2D(NoiseSampleLoc) + 1.f) / 2.f;
	
	// If used for pathing, the noise is multiplied exponentially to make high weights worth more, and clamped to above 0 to avoid negative weights 
	if (bSmoothForPathing)
	{
		return 1+FMath::Pow(x + 0.5, 6.f);
	}
	
	return x;
}

bool UPathGenerationBlueprintLibrary::IsPathValid(FTileCoord StartTile, FTileCoord GoalTile, TMap<FTileCoord, FTileCoord> Map)
{
	if (!Map.Contains(GoalTile))
	{
		return false;
	}

	FTileCoord CurrentTile = GoalTile;
	FTileCoord NextTile = Map[GoalTile];

	while (NextTile != StartTile)
	{
		CurrentTile = NextTile;

		if (!Map.Contains(CurrentTile)) { return false; }
		
		NextTile = Map[CurrentTile];
	}

	return true;
}

TMap<FTileCoord, FTileCoord> UPathGenerationBlueprintLibrary::AStarPathFind(const FTileCoord StartTile, const FTileCoord GoalTile,
                                                                            FVector2D NoiseOffset, TMap<FTileCoord, FTileCoord> ExistingPath)
{
	bool PathFound = false;
	TMap<FTileCoord, FTileCoord> Path;

	while (!PathFound)
	{
		TPriorityQueue<FTileCoord> Frontier;
		Frontier.Push(StartTile, 0);

		TMap<FTileCoord, FTileCoord> CameFrom;
		TMap<FTileCoord, float> CostSoFar;
		CostSoFar.Add(StartTile, 0);

		FVector GoalTileLocation = GoalTile.ToWorldLocation();

		while (!Frontier.IsEmpty())
		{
			const FTileCoord Current = Frontier.Pop();
			if (Current == GoalTile)
			{
				break;
			}

			for (FTileCoord Neighbour : FTileCoord::GetTilesInRadius(Current , 1))
			{
				TSet<FTileCoord> ExistingPathAndAdjacent;
				for (TTuple<FTileCoord, FTileCoord> Element : ExistingPath)
				{
					if (Element.Value == FTileCoord()) { continue; }
					ExistingPathAndAdjacent.Append(FTileCoord::GetTilesInRadius(Element.Value, 1));
				}
				if (ExistingPathAndAdjacent.Contains(Neighbour) && (Neighbour!=GoalTile))
				{
					continue;
				}
				// Find the weight of the neighbor tile. If the tile is in the exclusion list or is outside the boundary, returns -1
				// If the neighbor tile is the goal tile, ignores the impassable weight
				float NextWeight = GetNodeWeight(Neighbour, NoiseOffset, true);
			
				float NewCost = CostSoFar[Current] + NextWeight;
				if (!CostSoFar.Contains(Neighbour) || NewCost < CostSoFar[Neighbour])
				{
					CameFrom.Add(Neighbour, Current);
					CostSoFar.Add(Neighbour, NewCost);
					float Heuristic = FVector::Distance(GoalTileLocation, Neighbour.ToWorldLocation())/100.f;
					float Priority = NewCost + Heuristic;
					Frontier.Push(Neighbour, Priority);
				}
			}
		}

		if (IsPathValid(StartTile, GoalTile, CameFrom))
		{
			PathFound = true;

			FTileCoord CurrentTile = GoalTile;
			while (CurrentTile != StartTile)
			{
				FTileCoord CameFromTile = CameFrom[CurrentTile];
				Path.Add(CameFromTile, CurrentTile);
				CurrentTile = CameFromTile;
			}
		} else
		{
			UE_LOG(LogTemp, Fatal, TEXT("UPathGenerationBlueprintLibrary::AStarPathFind() - No path found."))
			break;
		}
	}

	return Path;
}






















