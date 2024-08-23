// Fill out your copyright notice in the Description page of Project Settings.


#include "AegisMapFactory.h"

#include "AegisGameMap.h"
#include "MapTile.h"
#include "Aegis/Structures/NexusBuilding/NexusBuilding.h"
#include "Aegis/Utilities/TPriorityQueue.h"
#include "MapTiles/MapTileData.h"

void UAegisMapFactory::PostInitProperties()
{
	UObject::PostInitProperties();

	if (!GetWorld()) { return; }

	TileHeight = 2 * TileRadius;
	TileWidth = FMath::Sqrt(3.f) * TileRadius;

	VerticalSpacing = 1.5 * TileRadius;
	HorizontalSpacing = TileWidth;

	OffsetQ = FVector(0, HorizontalSpacing, 0);
	OffsetR = FVector(VerticalSpacing, HorizontalSpacing / 2, 0);
}

TArray<FTileCoord> UAegisMapFactory::GetPathStartCoords(TMap<FTileCoord, FTileCoord> PathRoute)
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

UAegisGameMap* UAegisMapFactory::GenerateGameMap(const int PathLengthInNodes) const
{
	UE_LOG(LogTemp, Display, TEXT("UAegisMapFactory::GenerateGameMap - Map Generation Start!"));
	const FRandomStream RandomStream = FRandomStream(static_cast<int32>(FDateTime::Now().ToUnixTimestamp()));
	const FVector2D ElevationNoiseOffset = GetRandomNoiseOffset(RandomStream);
	const FVector2D PathingNoiseOffset = GetRandomNoiseOffset(RandomStream);
	const FVector2D TreeNoiseOffset = GetRandomNoiseOffset(RandomStream);
	const FVector2D StoneNoiseOffset = GetRandomNoiseOffset(RandomStream);

	// Generate the poisson nodes to build the path around
	UE_LOG(LogTemp, Display, TEXT("UAegisMapFactory::GenerateGameMap - Generating poisson data..."));
	const TArray<FTileCoord> PoissonNodeCoords = GeneratePoissonNodeCoords(4, 100, 5000, RandomStream);
	UE_LOG(LogTemp, Warning, TEXT("UAegisMapFactory::GenerateGameMap - Node count: %i"), PoissonNodeCoords.Num())
	UE_LOG(LogTemp, Display, TEXT("UAegisMapFactory::GenerateGameMap - Connecting poisson nodes via pseudo Delaunay Triangulation..."));
	const TMap<FTileCoord, TSet<FTileCoord>> PoissonNodeGraph = GeneratePseudoDelaunayTriangulation(PoissonNodeCoords, 8);

	// Generate a path from the graph
	UE_LOG(LogTemp, Display, TEXT("UAegisMapFactory::GenerateGameMap - Selecting nodes to path through"));
	const TArray<FTileCoord> PathNodes = SelectRandomPathThroughNodeGraph(PathLengthInNodes, PoissonNodeGraph, RandomStream);
	UE_LOG(LogTemp, Display, TEXT("UAegisMapFactory::GenerateGameMap - Generating path through selected nodes via A*..."));
	const TMap<FTileCoord, FTileCoord> Path = GenerateAStarPathThroughNodes(PathNodes, PathingNoiseOffset);

	// Generate MapTileData from the path and noise offsets...
	const TMap<FTileCoord, UMapTileData*> MapTilesData = GenerateMapTilesDataAroundPath(Path, ElevationNoiseOffset, TreeNoiseOffset, StoneNoiseOffset);

	// Spawn a NexusBuilding
	UE_LOG(LogTemp, Display, TEXT("UAegisMapFactory::GenerateGameMap - Creating Nexus building..."));
	ANexusBuilding* NexusBuilding = GetWorld()->SpawnActor<ANexusBuilding>(NexusBuildingBP, FVector(0, 0, 0), FRotator(0, 0, 0));

	// Set path start tile coords
	UE_LOG(LogTemp, Display, TEXT("UAegisMapFactory::GenerateGameMap - Getting path starting coords..."));
	const TArray<FTileCoord> PathStartTileCoords = GetPathStartCoords(Path);

	// Create map instance
	UE_LOG(LogTemp, Display, TEXT("UAegisMapFactory::GenerateGameMap - Creating Map instance..."));
	UAegisGameMap* Map = NewObject<UAegisGameMap>(GetWorld(), AegisGameMapClass);
	Map->PopulateGameMapData(MapTilesData, Path, PathStartTileCoords, NexusBuilding);

	// Set TilesToEnd of every tile
	UE_LOG(LogTemp, Display, TEXT("UAegisMapFactory::GenerateGameMap - Setting path properties..."));
	for (AMapTile* Tile : Map->GetTiles())
	{
		Tile->TilesToEnd = Map->GetNumOfTilesToEnd(Tile->TileCoord);
	}

	UE_LOG(LogTemp, Display, TEXT("UAegisMapFactory::GenerateGameMap - Map Generation Complete!"));
	return Map;
}

UAegisGameMap* UAegisMapFactory::GenerateWorldMap() const
{
	return nullptr;
}

FVector2D UAegisMapFactory::GetRandomNoiseOffset(const FRandomStream& RandomStream)
{
	return FVector2D(RandomStream.FRandRange(-100000.f, 100000.f), RandomStream.FRandRange(-100000.f, 100000.f));
}

TArray<FTileCoord> UAegisMapFactory::GeneratePoissonNodeCoords(const int32 PoissonDiskRadiusInMetres, const int32 GenerationRadiusInMetres,
	const int32 MaxSampleCount, const FRandomStream RandomStream)
{
	// Generate the raw poisson disk cluster data
	TArray<FVector2d> PoissonClusters = GetPoissonDiskClusters(PoissonDiskRadiusInMetres, GenerationRadiusInMetres, MaxSampleCount, RandomStream);

	// For every poisson disk, convert it to an FTileCoord to return
	TArray<FTileCoord> NodePoints;
	for (const FVector2d PoissonCluster : PoissonClusters)
	{
		NodePoints.Emplace(FTileCoord::FromWorldLocation(FVector(PoissonCluster.X, PoissonCluster.Y, 0) * 100));
	}

	// Sort by distance to centre
	NodePoints.Sort([](const FTileCoord& A, const FTileCoord& B)
	{
		//return FVector::Dist(A.ToWorldLocation(), FVector::ZeroVector) < FVector::Dist(B.ToWorldLocation(), FVector::ZeroVector);
		return FTileCoord::HexDistance(A, FTileCoord()) <  FTileCoord::HexDistance(B, FTileCoord());
	});
	
	return NodePoints;
}

TArray<FVector2D> UAegisMapFactory::GetPoissonDiskClusters(const int32 PoissonDiskRadiusInMetres, const int32 GenerationRadiusInMetres,
	const int32 MaxSampleCount, const FRandomStream RandomStream)
{
	// Shouldn't *really* use FTileCoord, but we basically just want to be able to compare pairs of integers, and FTileCoord already has that functionality,
	// We just have to map X->Q, Y->R, and ignore S
	TMap<FTileCoord, FVector2d> Grid;
	TArray<FVector2d> List;
	const float Bin = static_cast<float>(PoissonDiskRadiusInMetres) / FMath::Sqrt(2.f);

	// Place the initial point in the Grid and List
	constexpr float X = 0;
	constexpr float Y = 0;
	Grid.Add(FTileCoord(static_cast<int>(X / Bin), static_cast<int>(Y / Bin)), FVector2d(X, Y));
	List.Add(FVector2d(X, Y));

	// Loop until List is empty
	while (List.Num() > 0)
	{
		constexpr int Max_Trials = 30;
		// Pick a random point from List
		const int K = RandomStream.RandRange(0, List.Num() - 1);
		const float X0 = List[K].X;
		const float Y0 = List[K].Y;

		// Trials to place the next point
		bool FoundTrialPoint = false;
		for (int Trial = 0; Trial < Max_Trials; Trial++)
		{
			// Generate a trial point in the annulus r->2*r around x0,y0
			const float R = RandomStream.FRandRange(static_cast<float>(PoissonDiskRadiusInMetres), static_cast<float>(2 * PoissonDiskRadiusInMetres));
			const float Phi = RandomStream.FRandRange(0.f, static_cast<float>(2 * PI));
			const float TrialX = X0 + R * FMath::Cos(Phi);
			const float TrialY = Y0 + R * FMath::Sin(Phi);

			// Reject point if it is outside the GenerationRadius
			if (TrialX < -GenerationRadiusInMetres || TrialX > GenerationRadiusInMetres || TrialY < -GenerationRadiusInMetres || TrialY > GenerationRadiusInMetres)
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
		if (Grid.Num() >= MaxSampleCount)
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

TMap<FTileCoord, TSet<FTileCoord>> UAegisMapFactory::GeneratePseudoDelaunayTriangulation(const TArray<FTileCoord>& PoissonNodeCoords, const int PoissonRadius)
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

TArray<FTileCoord> UAegisMapFactory::SelectRandomPathThroughNodeGraph(const int32 PathLengthInNodes, const TMap<FTileCoord, TSet<FTileCoord>>& PoissonNodeGraph, FRandomStream RandomStream)
{
	if (PoissonNodeGraph.Num() < PathLengthInNodes)
	{
		UE_LOG(LogTemp, Error, TEXT("UAegisMapFactory::GenerateRandomPathThroughNodeGraph - Graph size is too small to find a path of length %i"), PathLengthInNodes);
		return TArray<FTileCoord>();
	}
	
	// The stack for DFS searching. Keeps track of the head node, and the path to that node at any time.
	TArray<TTuple<FTileCoord, TArray<FTileCoord>>> Stack;

	// NOTE - this DFS does not make use of an "explored"/"visited" list of nodes, as the suitable path is not necessarily the shortest path to the
	// end node, it is just searching for any path to any node that meets a certain length. This means that looping is allowed and only
	// backtracking is needed.
	
	TArray<FTileCoord> InitialPath = {FTileCoord()};
	const TTuple<FTileCoord, TArray<FTileCoord>> InitialValue = TTuple<FTileCoord, TArray<FTileCoord>>(FTileCoord(),InitialPath);
	Stack.Add(InitialValue);
	
	while (Stack.Num() != 0)
	{
		// Get the top node and the path to it
		const TTuple<FTileCoord, TArray<FTileCoord>> Head = Stack.Pop();

		// Check if this head is a suitable return node, and return if it is
		if (Head.Value.Num() == PathLengthInNodes)
		{
			return Head.Value;
		}

		// The path is not long enough, get all neighbouring nodes
		TArray<FTileCoord> NeighbourNodes = PoissonNodeGraph[Head.Key].Array();
		NeighbourNodes.Sort([RandomStream](FTileCoord A, FTileCoord B)
		{
			//return RandomStream.FRand() < 0.5;
			return FTileCoord::HexDistance(A, FTileCoord()) > FTileCoord::HexDistance(B, FTileCoord());
		});

		for (FTileCoord NeighbourNode : NeighbourNodes)
		{
			if (Head.Value.Contains(NeighbourNode))
			{
				// Node exists in the path so far, skip
				continue;
			}
			
			// Create the path to neighbour by adding this tile to the path to its parent
			TArray<FTileCoord> PathToNeighbour = Head.Value;
			PathToNeighbour.Add(NeighbourNode);
			
			TTuple<FTileCoord, TArray<FTileCoord>> StackValue;
			StackValue.Key = NeighbourNode;
			StackValue.Value = PathToNeighbour;

			Stack.Add(StackValue);
		}
	}

	UE_LOG(LogTemp, Error, TEXT("UAegisMapFactory::GenerateRandomPathThroughNodeGraph - Could not find path through node graph!!"));
	return TArray<FTileCoord>();
}

TMap<FTileCoord, FTileCoord> UAegisMapFactory::GenerateAStarPathThroughNodes(const TArray<FTileCoord>& PathNodes, FVector2D PathingNoiseOffset)
{
	TMap<FTileCoord, FTileCoord> Path;
	Path.Add(FTileCoord(), FTileCoord());
	
	// For every node in the path array, use A* to connect it to the previous node
	for (int NodeIndex = 0; NodeIndex < PathNodes.Num()-1; NodeIndex++)
	{
		const FTileCoord StartTile = PathNodes[NodeIndex];
		const FTileCoord EndTile = PathNodes[NodeIndex+1];

		TMap<FTileCoord, FTileCoord> PathFromEndToStart = AStarPathFind(EndTile, StartTile, PathingNoiseOffset, Path);
		Path.Append(PathFromEndToStart);
	}
	return Path;
}

TMap<FTileCoord, FTileCoord> UAegisMapFactory::AStarPathFind(const FTileCoord StartTile, const FTileCoord GoalTile, FVector2D PathingNoiseOffset,
	const TMap<FTileCoord, FTileCoord>& ExistingPath)
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

TMap<FTileCoord, UMapTileData*> UAegisMapFactory::GenerateMapTilesDataAroundPath(const TMap<FTileCoord, FTileCoord>& Path,
	FVector2D ElevationNoiseOffset, FVector2D TreeNoiseOffset, FVector2D StoneNoiseOffset)
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

	// Set Terrain and Elevation
	for (TTuple<FTileCoord, UMapTileData*> Elem : MapTilesData)
	{
		// If the tile is part of the path, set some path default values
		if (Path.Contains(Elem.Key))
		{
			Elem.Value->bIsPath = true;
			Elem.Value->Elevation = 0;

			continue;
		}

		if (Elem.Value->ElevationNoise < WaterLimit)
		{
			Elem.Value->Elevation = 0;
			Elem.Value->TerrainType = Water;
		}
		else if (Elem.Value->ElevationNoise < Grass1Limit)
		{
			Elem.Value->Elevation = 0;
			Elem.Value->TerrainType = Grass;
		}
		else if (Elem.Value->ElevationNoise < Grass2Limit)
		{
			Elem.Value->Elevation = 1;
			Elem.Value->TerrainType = Grass;
		}
		else if (Elem.Value->ElevationNoise < Grass3Limit)
		{
			Elem.Value->Elevation = 2;
			Elem.Value->TerrainType = Grass;
		}
		else if (Elem.Value->ElevationNoise < Grass4Limit)
		{
			Elem.Value->Elevation = 3;
			Elem.Value->TerrainType = Grass;
		}
		else
		{
			Elem.Value->Elevation = 4;
			Elem.Value->TerrainType = Cliff;
		}

		// Set max elevation (and adjust terrain type if necessary) based on distance to the start and end path tiles
		//const int MaxElevation = FTileCoord::HexDistance(FTileCoord(), Elem.Key);
		const int MaxElevation = FTileCoord::HexDistanceToTiles(PathTiles, Elem.Key);
		Elem.Value->Elevation = FMath::Min(Elem.Value->Elevation, MaxElevation);

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
	}

	return MapTilesData;
}

bool UAegisMapFactory::IsPathValid(const FTileCoord StartTile, const FTileCoord GoalTile, const TMap<FTileCoord, FTileCoord>& Path)
{
	if (!Path.Contains(GoalTile))
	{
		return false;
	}

	FTileCoord CurrentTile = GoalTile;
	FTileCoord NextTile = Path[GoalTile];

	while (NextTile != StartTile)
	{
		CurrentTile = NextTile;

		if (!Path.Contains(CurrentTile)) { return false; }

		NextTile = Path[CurrentTile];
	}

	return true;
}

float UAegisMapFactory::GetNodeWeight(const FTileCoord Tile, const FVector2D NoiseOffset, const bool bDistortNoise, const int32 PerlinScale)
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
