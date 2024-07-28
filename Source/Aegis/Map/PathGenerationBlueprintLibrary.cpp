// Fill out your copyright notice in the Description page of Project Settings.


#include "PathGenerationBlueprintLibrary.h"
#include "Aegis/Utilities/TPriorityQueue.h"


TArray<FVector2d> UPathGenerationBlueprintLibrary::GetBlueNoiseClusters(const int GenerationRadius, const int PoissonRadius, const int SamplesCount,
                                                                        const FRandomStream RandomStream)
{
	// ReSharper disable once CppTooWideScope
	// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
	constexpr int MAX_TRIALS = 30;

	// Shouldn't *really* use FTileCoord, but we basically just want to be able to compare pairs of integers, and FTileCoord already has that functionality,
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
		const int K = RandomStream.RandRange(0, List.Num() - 1);
		const float X0 = List[K].X;
		const float Y0 = List[K].Y;

		// Trials to place the next point
		bool FoundTrialPoint = false;
		for (int Trial = 0; Trial < MAX_TRIALS; Trial++)
		{
			// Generate a trial point in the annulus r->2*r around x0,y0
			const float R = RandomStream.FRandRange(static_cast<float>(PoissonRadius), static_cast<float>(2 * PoissonRadius));
			const float Phi = RandomStream.FRandRange(0.f, static_cast<float>(2 * PI));
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

float UPathGenerationBlueprintLibrary::GetNodeWeight(const FTileCoord Tile, const FVector2D NoiseOffset, const bool bDistortNoise,
                                                     const int32 PerlinScale)
{
	// Larger Perlin Scale means more spread out and smoother (1000 is a good baseline)
	// Smaller Perlin Scale means higher frequency and more random (e.g. 10 for pathfinding randomness)
	//int PerlinScale = 1000;
	const FVector Location = Tile.ToWorldLocation();
	FVector2D NoiseSampleLoc = FVector2D(Location.X / PerlinScale, Location.Y / PerlinScale) + (NoiseOffset);
	if (!bDistortNoise)
	{
		const float x = NoiseSampleLoc.X;
		const float y = NoiseSampleLoc.Y;
		// Distort the noise if not used for pathing
		const float XDistortion = 0 * FMath::PerlinNoise2D(FVector2D(4.7 * (x + 2.3), 4.7 * (y + 2.9)));
		const float YDistortion = 0 * FMath::PerlinNoise2D(FVector2D(4.7 * (x - 3.1), 4.7 * (y - 4.3)));

		NoiseSampleLoc = FVector2D(x + XDistortion, y + YDistortion);
	}
	const float x = (FMath::PerlinNoise2D(NoiseSampleLoc) + 1.f) / 2.f;

	// If used for pathing, the noise is multiplied exponentially to make high weights worth more, and clamped to above 0 to avoid negative weights 
	if (bDistortNoise)
	{
		return 1 + FMath::Pow(x + 0.5, 10.f);
	}

	return x;
}

bool UPathGenerationBlueprintLibrary::IsPathValid(const FTileCoord StartTile, const FTileCoord GoalTile, TMap<FTileCoord, FTileCoord> Map)
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
                                                                            FVector2D PathingNoiseOffset, TMap<FTileCoord, FTileCoord> ExistingPath)
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

			for (FTileCoord Neighbour : FTileCoord::GetTilesInRadius(Current, 1))
			{
				TSet<FTileCoord> ExistingPathAndAdjacent;
				for (TTuple<FTileCoord, FTileCoord> Element : ExistingPath)
				{
					if (Element.Value == FTileCoord()) { continue; }
					ExistingPathAndAdjacent.Append(FTileCoord::GetTilesInRadius(Element.Value, 1));
				}
				if (ExistingPathAndAdjacent.Contains(Neighbour) && (Neighbour != GoalTile))
				{
					continue;
				}
				// Find the weight of the neighbor tile. If the tile is in the exclusion list or is outside the boundary, returns -1
				// If the neighbor tile is the goal tile, ignores the impassable weight
				float NextWeight = GetNodeWeight(Neighbour, PathingNoiseOffset, true, 10);

				float NewCost = CostSoFar[Current] + NextWeight;
				if (!CostSoFar.Contains(Neighbour) || NewCost < CostSoFar[Neighbour])
				{
					CameFrom.Add(Neighbour, Current);
					CostSoFar.Add(Neighbour, NewCost);
					float Heuristic = FVector::Distance(GoalTileLocation, Neighbour.ToWorldLocation()) / 100.f;
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
		}
		else
		{
			UE_LOG(LogTemp, Fatal, TEXT("UPathGenerationBlueprintLibrary::AStarPathFind() - No path found."))
			break;
		}
	}

	return Path;
}

TArray<FTileCoord> UPathGenerationBlueprintLibrary::GetPoissonClusterCoords(const int GenerationRadius, const int Poisson_Radius, const int MaxSamplesCount,
                                                                            const FRandomStream RandomStream)
{
	TArray<FVector2d> PoissonClusters = GetBlueNoiseClusters(GenerationRadius, Poisson_Radius, MaxSamplesCount, RandomStream);
	TArray<FTileCoord> NodePoints;
	for (const FVector2d PoissonCluster : PoissonClusters)
	{
		NodePoints.Emplace(FTileCoord::PixelToHex(FVector(PoissonCluster.X, PoissonCluster.Y, 0) * 100));
	}
	
	NodePoints.Sort([](const FTileCoord& A, const FTileCoord& B)
		{
			//return FVector::Dist(A.ToWorldLocation(), FVector::ZeroVector) < FVector::Dist(B.ToWorldLocation(), FVector::ZeroVector);
			return FTileCoord::HexDistance(A, FTileCoord()) <  FTileCoord::HexDistance(B, FTileCoord());
		});
	
	return NodePoints;
}

TMap<FTileCoord, FTileCoord> UPathGenerationBlueprintLibrary::GenerateGreedyPoissonPath(const int PathNodeLength,
                                                                                        const TArray<FTileCoord>& PoissonNodeCoords,
                                                                                        const FVector2d ElevationNoiseOffset,
                                                                                        const FRandomStream RandomStream)
{
	TMap<FTileCoord, FTileCoord> Path;

	// Get the Path Nodes using a Greedy closest Node search of the poisson clusters
	TArray<FTileCoord> NodesInPathSoFar;
	NodesInPathSoFar.Add(FTileCoord(0, 0));
	for (int i = 0; i < PathNodeLength; i++)
	{
		FTileCoord Head = NodesInPathSoFar.Top();

		// Find the closest node to the Head of the Path
		FTileCoord Closest = FTileCoord(INT_MAX, INT_MAX);
		for (FTileCoord Point : PoissonNodeCoords)
		{
			if ((Point == Head) || NodesInPathSoFar.Contains(Point)) { continue; }

			const float DistanceToHead = FTileCoord::HexDistance(Point, Head);
			if (DistanceToHead < FTileCoord::HexDistance(Closest, Head))
			{
				Closest = Point;
			}
		}
		NodesInPathSoFar.Emplace(Closest);
	}

	// For every node in the Path array, use A* to connect it to the previous node
	Path.Add(FTileCoord(), FTileCoord());
	for (int StartTileIndex = 1; StartTileIndex < NodesInPathSoFar.Num(); StartTileIndex++)
	{
		const FTileCoord StartTile = NodesInPathSoFar[StartTileIndex];
		const FTileCoord GoalTile = NodesInPathSoFar[StartTileIndex - 1];

		TMap<FTileCoord, FTileCoord> PathFromGoalToStart = AStarPathFind(StartTile, GoalTile, ElevationNoiseOffset, Path);
		Path.Append(PathFromGoalToStart);
	}

	return Path;
}

TMap<FTileCoord, UMapTileData*> UPathGenerationBlueprintLibrary::GenerateMapTilesData(const TMap<FTileCoord, FTileCoord>& Path,
                                                                                      const FVector2D ElevationNoiseOffset, FVector2D TreeNoiseOffset,
                                                                                      FVector2D StoneNoiseOffset, FRandomStream RandomStream)
{
	TArray<float> ElevationNoiseDistribution;
	TArray<float> TreeNoiseDistribution;
	TArray<float> StoneNoiseDistribution;
	TMap<FTileCoord, UMapTileData*> MapTilesData;

	TArray<FTileCoord> PathTiles;
	Path.GenerateKeyArray(PathTiles);

	int TargetRadius = 1;
	int TilesInTargetRadius = FTileCoord::GetTilesInRadius(PathTiles, TargetRadius).Num();
	while (TilesInTargetRadius < (Path.Num() * 20))
	{
		TargetRadius++;
		TilesInTargetRadius = FTileCoord::GetTilesInRadius(PathTiles, TargetRadius).Num();
	}

	for (const FTileCoord Coord : FTileCoord::GetTilesInRadius(PathTiles, TargetRadius))
	{
		UMapTileData* MapTileData = NewObject<UMapTileData>();

		const float ElevationGradient = (GetNodeWeight(Coord, ElevationNoiseOffset, false, 1000) + 1.f) / 2.f;
		MapTileData->ElevationNoise = ElevationGradient;
		ElevationNoiseDistribution.Emplace(ElevationGradient);

		const float TreeGradient = (GetNodeWeight(Coord, TreeNoiseOffset, false, 700) + 1.f) / 2.f;
		MapTileData->TreeNoise = TreeGradient;
		//TreeNoiseDistribution.Emplace(TreeGradient);

		const float StoneGradient = (GetNodeWeight(Coord, StoneNoiseOffset, false, 100) + 1.f) / 2.f;
		MapTileData->StoneNoise = StoneGradient;
		//StoneNoiseDistribution.Emplace(StoneGradient);

		MapTilesData.Add(Coord, MapTileData);
	}

	ElevationNoiseDistribution.Sort();

	const float WaterLimit = ElevationNoiseDistribution[static_cast<int>((ElevationNoiseDistribution.Num() - 1) * 0.1)];
	const float Grass1Limit = ElevationNoiseDistribution[static_cast<int>((ElevationNoiseDistribution.Num() - 1) * 0.8)];
	const float Grass2Limit = ElevationNoiseDistribution[static_cast<int>((ElevationNoiseDistribution.Num() - 1) * 0.9)];
	const float Grass3Limit = ElevationNoiseDistribution[static_cast<int>((ElevationNoiseDistribution.Num() - 1) * 0.95)];
	const float Grass4Limit = ElevationNoiseDistribution[static_cast<int>((ElevationNoiseDistribution.Num() - 1) * 0.98)];
	const float CliffLimit = ElevationNoiseDistribution[static_cast<int>(ElevationNoiseDistribution.Num() - 1 * 1)];

	TMap<int, ETerrainType> ElevationTerrainMap;
	ElevationTerrainMap.Add(0, Water);
	ElevationTerrainMap.Add(1, Grass);
	ElevationTerrainMap.Add(2, Grass);
	ElevationTerrainMap.Add(3, Grass);
	ElevationTerrainMap.Add(4, Grass);
	ElevationTerrainMap.Add(5, Cliff);

	// Set Terrain and Elevation
	for (TTuple<FTileCoord, UMapTileData*> Elem : MapTilesData)
	{
		// If the tile is part of the path, set some path default values
		if (Path.Contains(Elem.Key))
		{
			Elem.Value->bIsPath = true;
			Elem.Value->Elevation = 0.5;

			continue;
		}

		if (Elem.Value->ElevationNoise < WaterLimit)
		{
			Elem.Value->Elevation = 0;
		}
		else if (Elem.Value->ElevationNoise < Grass1Limit)
		{
			Elem.Value->Elevation = 1;
		}
		else if (Elem.Value->ElevationNoise < Grass2Limit)
		{
			Elem.Value->Elevation = 2;
		}
		else if (Elem.Value->ElevationNoise < Grass3Limit)
		{
			Elem.Value->Elevation = 3;
		}
		else if (Elem.Value->ElevationNoise < Grass4Limit)
		{
			Elem.Value->Elevation = 4;
		}
		else
		{
			Elem.Value->Elevation = 5;
		}

		// Set max elevation (and adjust terrain type if necessary) based on distance to the start and end path tiles
		//const int MaxElevation = FTileCoord::HexDistance(FTileCoord(), Elem.Key);
		const int MaxElevation = FTileCoord::HexDistanceToTiles(PathTiles, Elem.Key);
		Elem.Value->Elevation = FMath::Min(Elem.Value->Elevation, MaxElevation);

		Elem.Value->TerrainType = ElevationTerrainMap[Elem.Value->Elevation];
	}


	auto CanTileHaveTree = [](const UMapTileData* MapTileData)
	{
		const TArray<ETerrainType> TreeAllowedTerrain = {Grass};
		return (!MapTileData->bIsPath && TreeAllowedTerrain.Contains(MapTileData->TerrainType));
	};
	auto CanTileHaveStone = [](const UMapTileData* MapTileData)
	{
		const TArray<ETerrainType> StoneAllowedTerrain = {Grass, Cliff};
		return (!MapTileData->bIsPath && StoneAllowedTerrain.Contains(MapTileData->TerrainType));
	};

	// Set Resources
	for (TTuple<FTileCoord, UMapTileData*> Elem : MapTilesData)
	{
		// Only count towards tree noise distribution if the tile can have a tree on it (e.g. trees cannot be on water)
		if (CanTileHaveTree(Elem.Value))
		{
			TreeNoiseDistribution.Emplace(Elem.Value->TreeNoise);
		}
		if (CanTileHaveStone(Elem.Value))
		{
			StoneNoiseDistribution.Emplace(Elem.Value->StoneNoise);
		}
	}
	TreeNoiseDistribution.Sort();
	StoneNoiseDistribution.Sort();
	const float TreeLimit = ElevationNoiseDistribution[static_cast<int>((TreeNoiseDistribution.Num() - 1) * 0.2)];
	const float StoneLimit = ElevationNoiseDistribution[static_cast<int>((TreeNoiseDistribution.Num() - 1) * 0.1)];
	for (TTuple<FTileCoord, UMapTileData*> Elem : MapTilesData)
	{
		if (CanTileHaveTree(Elem.Value) && Elem.Value->TreeNoise < TreeLimit)
		{
			Elem.Value->ResourceType = Tree;
		}
		if (CanTileHaveStone(Elem.Value) && Elem.Value->StoneNoise < StoneLimit)
		{
			Elem.Value->ResourceType = Stone;
		}
		if (CanTileHaveStone(Elem.Value) && CanTileHaveTree(Elem.Value) && Elem.Value->TreeNoise < TreeLimit && Elem.Value->StoneNoise < StoneLimit)
		{
			Elem.Value->ResourceType = TreeStone;
		}
	}

	return MapTilesData;
}

bool AreAllBlobsComplete(const TMap<FTileCoord, bool>& Map)
{
	for (const TPair<FTileCoord, bool> Pair : Map)
	{
		if (Pair.Value == false)
		{
			return false;
		}
	}
	return true;
}

TMap<FTileCoord, TSet<FTileCoord>> UPathGenerationBlueprintLibrary::GeneratePoissonNodeGraph(const TArray<FTileCoord>& PoissonNodeCoords, const int PoissonRadius, const int GenerationRadius, UWorld* World)
{
	TMap<FTileCoord, TSet<FTileCoord>> PoissonNodeGraph;

	// For each node, create a blob containing all the tiles adjacent to the blob
	TMap<FTileCoord, TSet<FTileCoord>> Blobs;
	TMap<FTileCoord, bool> IsBlobComplete;
	for (FTileCoord Node : PoissonNodeCoords)
	{
		//TSet<FTileCoord> InitialBlob = TSet(FTileCoord::GetTilesInRadius(Node, FMath::RoundToNegativeInfinity(static_cast<float>(PoissonRadius-1)/2.f)));
		TSet<FTileCoord> InitialBlob = TSet(FTileCoord::GetTilesInRadius(Node, 1));
		Blobs.Add(Node, InitialBlob);
		IsBlobComplete.Add(Node, false);
	}
	TSet<FTileCoord> BlobbedTiles;
	for (TPair<FTileCoord, TSet<FTileCoord>> Pair : Blobs)
	{
		BlobbedTiles.Append(Pair.Value);
	}
	
	// For each un-complete blob, add the closest adjacent un-blobbed tile.
		// If no such adjacent tile exists, mark node/blob as complete
		// If the closest adjacent tile is beyond 2xPoissonRadius in distance, mark node as complete
	for (int RangeToClaim = 1; RangeToClaim < PoissonRadius/2; RangeToClaim++)
	{
		for (TPair<FTileCoord, TSet<FTileCoord>> Pair : Blobs)
		{
			if (IsBlobComplete[Pair.Key]) { continue; }

			// Find all adjacent tiles to the blob
			TSet<FTileCoord> Blob = Pair.Value;
			TSet<FTileCoord> BlobAdjacent = TSet(FTileCoord::GetTilesInRadius(Blob.Array(), RangeToClaim));
			for (FTileCoord BlobTile : Blob) { BlobAdjacent.Remove(BlobTile); }

			// Remove any tiles that exist in another blob
			TSet<FTileCoord> CleanedBlobAdjacent; 
			for (FTileCoord AdjacentTile : BlobAdjacent)
			{
				if (!BlobbedTiles.Contains(AdjacentTile))
				{
					CleanedBlobAdjacent.Add(AdjacentTile);
				}
			}

			if (CleanedBlobAdjacent.Num() == 0)
			{
				IsBlobComplete[Pair.Key] = true;
				continue;
			}
			
			// Add to blob
			for (FTileCoord AdjacentTile : CleanedBlobAdjacent)
			{
				Blobs[Pair.Key].Add(AdjacentTile);
				BlobbedTiles.Add(AdjacentTile);
			}
		}
	}
	
	// For each blob
		// Find all adjacent tiles to the blob not in the blob
		// For each of these adjacent tiles, get the blob / node it is part of, and add it to the set for this blob
		// Add the set to the PoissonNodeGraph
	for (TPair<FTileCoord, TSet<FTileCoord>> Pair : Blobs)
	{
		TSet<FTileCoord> AdjacentNodes;
		
		TSet<FTileCoord> AdjacentToBlob = TSet(FTileCoord::GetTilesInRadius(Pair.Value.Array(), 1));
		for (FTileCoord BlobTile : Pair.Value)
		{
			AdjacentToBlob.Remove(BlobTile);
		}

		for (FTileCoord AdjacentTile : AdjacentToBlob)
		{
			// Find which blob this tile is part of, and add the parent node to the adjacent nodes
			for (TPair<FTileCoord, TSet<FTileCoord>> PossibleAdjacentBlob : Blobs)
			{
				if (PossibleAdjacentBlob.Value.Contains(AdjacentTile))
				{
					AdjacentNodes.Add(PossibleAdjacentBlob.Key);
					break;
				}
			}
		}
		
		PoissonNodeGraph.Add(Pair.Key, AdjacentNodes);
	}
	
	// return complete set of connected nodes
	return PoissonNodeGraph;
}
