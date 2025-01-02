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
	const int32 Seed = static_cast<int32>(FDateTime::Now().ToUnixTimestamp());
	//constexpr int32 Seed = 1735852301;
	UE_LOG(LogTemp, Display, TEXT("UAegisMapFactory::GenerateGameMap - Map Generation Started! Seed: %i"), Seed);
	const FDateTime TimeSTamp_Start = FDateTime::Now();

	const FRandomStream RandomStream = FRandomStream(Seed);
	const FVector2D ElevationNoiseOffset = GetRandomNoiseOffset(RandomStream);
	const FVector2D PathingNoiseOffset = GetRandomNoiseOffset(RandomStream);
	const FVector2D TreeNoiseOffset = GetRandomNoiseOffset(RandomStream);
	const FVector2D StoneNoiseOffset = GetRandomNoiseOffset(RandomStream);
	const FDateTime TimeStamp_GeneratedRandomOffsets = FDateTime::Now();

	// Generate the poisson nodes to build the path around
	const TArray<FTileCoord> PoissonNodeCoords = GeneratePoissonNodeCoords(8, 100, 5000, RandomStream);
	const FDateTime TimeStamp_GeneratedPoissonNodes = FDateTime::Now();
	UE_LOG(LogTemp, Display, TEXT("UAegisMapFactory::GenerateGameMap - Generated Poisson nodes in %fms"), (TimeStamp_GeneratedPoissonNodes - TimeStamp_GeneratedRandomOffsets).GetTotalMilliseconds());
	
	//const TMap<FTileCoord, TSet<FTileCoord>> PoissonNodeGraph = GeneratePseudoDelaunayTriangulation(PoissonNodeCoords, 8);
	const TMap<FTileCoord, TSet<FTileCoord>> PoissonNodeGraph = GenerateDelaunayTriangulation(PoissonNodeCoords);
	const FDateTime TimeStamp_GeneratedPoissonNodeGraph = FDateTime::Now();
	UE_LOG(LogTemp, Display, TEXT("UAegisMapFactory::GenerateGameMap - Generated Pseudo Delaunay Triangulation nodes in %fms"), (TimeStamp_GeneratedPoissonNodeGraph - TimeStamp_GeneratedPoissonNodes).GetTotalMilliseconds());
		
	// Draw lines between each connected node
	// for (TPair<FTileCoord, TSet<FTileCoord>> Pair : PoissonNodeGraph)
	// {
	// 	for (FTileCoord AdjacentNode : Pair.Value)
	// 	{
	// 		FVector Start = Pair.Key.ToWorldLocation() + FVector(0, 0, 400); // Origin
	// 		FVector End = AdjacentNode.ToWorldLocation() + FVector(0, 0, 400); // Endpoint
	//
	// 		FColor Color = FColor::Red; // Line color
	//
	// 		// Draw the debug line
	// 		DrawDebugLine(GetWorld(), Start, End, Color, true, -1, 0, 5);
	// 	}
	// }
	
	const FDateTime TimeStamp_DrawnEdgesInWorld = FDateTime::Now();
	UE_LOG(LogTemp, Display, TEXT("UAegisMapFactory::GenerateGameMap - Drawn node graph in world in %fms"), (TimeStamp_DrawnEdgesInWorld - TimeStamp_GeneratedPoissonNodeGraph).GetTotalMilliseconds());

	// Generate a path from the graph
	const TArray<FTileCoord> PathNodes = SelectRandomPathThroughNodeGraph(PathLengthInNodes, PoissonNodeGraph, RandomStream);
	const FDateTime TimeStamp_SelectPathNodes = FDateTime::Now();
	UE_LOG(LogTemp, Display, TEXT("UAegisMapFactory::GenerateGameMap - Selected path poisson nodes in %fms"), (TimeStamp_SelectPathNodes - TimeStamp_DrawnEdgesInWorld).GetTotalMilliseconds());
	
	const TMap<FTileCoord, FTileCoord> Path = GenerateAStarPathThroughNodes(PathNodes, PathingNoiseOffset);
	const FDateTime TimeStamp_GenerateAStarPath = FDateTime::Now();
	UE_LOG(LogTemp, Display, TEXT("UAegisMapFactory::GenerateGameMap - Generated path through selected nodes via A* in %fms"), (TimeStamp_GenerateAStarPath - TimeStamp_SelectPathNodes).GetTotalMilliseconds());

	// Generate MapTileData from the path and noise offsets...
	const TMap<FTileCoord, UMapTileData*> MapTilesData = GenerateMapTilesDataAroundPath(Path, ElevationNoiseOffset, TreeNoiseOffset, StoneNoiseOffset);
	const FDateTime TimeStamp_GeneratedMapTileData = FDateTime::Now();
	UE_LOG(LogTemp, Display, TEXT("UAegisMapFactory::GenerateGameMap - Generated map tile data in %fms"), (TimeStamp_GeneratedMapTileData - TimeStamp_GenerateAStarPath).GetTotalMilliseconds());
	
	// Spawn a NexusBuilding
	ANexusBuilding* NexusBuilding = GetWorld()->SpawnActor<ANexusBuilding>(NexusBuildingBP, FVector(0, 0, 0), FRotator(0, 0, 0));
	const FDateTime TimeStamp_CreatedNexusBuilding = FDateTime::Now();
	UE_LOG(LogTemp, Display, TEXT("UAegisMapFactory::GenerateGameMap - Created Nexus building in %fms"), (TimeStamp_CreatedNexusBuilding - TimeStamp_GeneratedMapTileData).GetTotalMilliseconds());

	// Set path start tile coords
	const TArray<FTileCoord> PathStartTileCoords = GetPathStartCoords(Path);
	const FDateTime TimeStamp_GotPathStartCoords = FDateTime::Now();
	UE_LOG(LogTemp, Display, TEXT("UAegisMapFactory::GenerateGameMap - Got path starting coords in %fms"), (TimeStamp_GotPathStartCoords - TimeStamp_CreatedNexusBuilding).GetTotalMilliseconds());

	// Create map instance
	UAegisGameMap* Map = NewObject<UAegisGameMap>(GetWorld(), AegisGameMapClass);
	Map->PopulateGameMapData(MapTilesData, Path, PathStartTileCoords, NexusBuilding);
	const FDateTime TimeStamp_CreateMapInstance = FDateTime::Now();
	UE_LOG(LogTemp, Display, TEXT("UAegisMapFactory::GenerateGameMap - Created map instance in %fms"), (TimeStamp_CreateMapInstance - TimeStamp_GotPathStartCoords).GetTotalMilliseconds());

	// Set TilesToEnd of every tile
	TArray<FTileCoord> PathCoords;
	Path.GenerateKeyArray(PathCoords);
	for (FTileCoord PathTile : PathCoords)
	{
		Map->GetTile(PathTile)->TilesToEnd = Map->GetNumOfTilesToEnd(PathTile);
	}
	const FDateTime TimeStamp_SetTilesToEnd = FDateTime::Now();
	UE_LOG(LogTemp, Display, TEXT("UAegisMapFactory::GenerateGameMap - Set path 'TilesToEnd' in %fms"), (TimeStamp_SetTilesToEnd - TimeStamp_CreateMapInstance).GetTotalMilliseconds());

	UE_LOG(LogTemp, Display, TEXT("UAegisMapFactory::GenerateGameMap - Map Generation Complete in %fms!"), (TimeStamp_SetTilesToEnd - TimeSTamp_Start).GetTotalMilliseconds());
	return Map;
}

UAegisWorldMap* UAegisMapFactory::GenerateWorldMap() const
{
	const int32 Seed = static_cast<int32>(FDateTime::Now().ToUnixTimestamp());
	const FRandomStream RandomStream = FRandomStream(Seed);
	const FVector2D ElevationNoiseOffset = GetRandomNoiseOffset(RandomStream);
	const FVector2D PathingNoiseOffset = GetRandomNoiseOffset(RandomStream);
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
	
	UE_LOG(LogTemp, Warning, TEXT("UAegisMapFactory::GeneratePoissonNodeCoords - Poisson node count: %i"), NodePoints.Num())
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

TMap<FTileCoord, TSet<FTileCoord>> UAegisMapFactory::GenerateDelaunayTriangulation(const TArray<FTileCoord>& PoissonNodeCoords)
{
	std::vector<Point> points;
	TMap<Point, FTileCoord> PointToCoord;
	for (FTileCoord Node : PoissonNodeCoords)
	{
		FVector NodeLoc = Node.ToWorldLocation();
		Point p = Point(NodeLoc.X, NodeLoc.Y);
		PointToCoord.Add(p, Node);
		points.push_back(p);
	}

	if (points.size() < 3)
	{
		return TMap<FTileCoord, TSet<FTileCoord>>();
	}
	auto xmin = points[0].x;
	auto xmax = xmin;
	auto ymin = points[0].y;
	auto ymax = ymin;
	for (auto const& pt : points)
	{
		xmin = std::min(xmin, pt.x);
		xmax = std::max(xmax, pt.x);
		ymin = std::min(ymin, pt.y);
		ymax = std::max(ymax, pt.y);
	}

	const auto dx = xmax - xmin;
	const auto dy = ymax - ymin;
	const auto dmax = std::max(dx, dy);
	const auto midx = (xmin + xmax) / static_cast<float>(2.);
	const auto midy = (ymin + ymax) / static_cast<float>(2.);

	/* Init Delaunay triangulation. */
	Delaunay d = Delaunay();

	const auto p0 = Point{midx - 20 * dmax, midy - dmax};
	const auto p1 = Point{midx, midy + 20 * dmax};
	const auto p2 = Point{midx + 20 * dmax, midy - dmax};
	d.triangles.emplace_back(Triangle{p0, p1, p2});

	for (auto const& pt : points)
	{
		std::vector<Edge> edges;
		std::vector<Triangle> tmps;
		for (auto const& tri : d.triangles)
		{
			/* Check if the point is inside the triangle circumcircle. */
			const auto dist = (tri.circle.x - pt.x) * (tri.circle.x - pt.x) +
				(tri.circle.y - pt.y) * (tri.circle.y - pt.y);
			if ((dist - tri.circle.radius) <= 1e-4)
			{
				edges.push_back(tri.e0);
				edges.push_back(tri.e1);
				edges.push_back(tri.e2);
			}
			else
			{
				tmps.push_back(tri);
			}
		}

		/* Delete duplicate edges. */
		std::vector<bool> remove(edges.size(), false);
		for (auto it1 = edges.begin(); it1 != edges.end(); ++it1)
		{
			for (auto it2 = edges.begin(); it2 != edges.end(); ++it2)
			{
				if (it1 == it2)
				{
					continue;
				}
				if (*it1 == *it2)
				{
					remove[std::distance(edges.begin(), it1)] = true;
					remove[std::distance(edges.begin(), it2)] = true;
				}
			}
		}

		edges.erase(
			std::remove_if(edges.begin(), edges.end(),
			               [&](auto const& e) { return remove[&e - &edges[0]]; }),
			edges.end());

		/* Update triangulation. */
		for (auto const& e : edges)
		{
			tmps.push_back({e.p0, e.p1, {pt.x, pt.y}});
		}
		d.triangles = tmps;
	}

	/* Remove original super triangle. */
	d.triangles.erase(
		std::remove_if(d.triangles.begin(), d.triangles.end(),
			[&](auto const& tri)
			{
			    return ((tri.p0 == p0 || tri.p1 == p0 || tri.p2 == p0) ||
			        (tri.p0 == p1 || tri.p1 == p1 || tri.p2 == p1) ||
			        (tri.p0 == p2 || tri.p1 == p2 || tri.p2 == p2));
			}),
		d.triangles.end());

	/* Add edges. */
	for (auto const& tri : d.triangles)
	{
		d.edges.push_back(tri.e0);
		d.edges.push_back(tri.e1);
		d.edges.push_back(tri.e2);
	}

	TMap<FTileCoord, TSet<FTileCoord>> OutTriangulation;
	for (Edge Edge : d.edges)
	{
		FTileCoord CoordA = PointToCoord[Edge.p0];
		FTileCoord CoordB = PointToCoord[Edge.p1];
		if (!OutTriangulation.Contains(CoordA))
		{
			OutTriangulation.Add(CoordA, TSet<FTileCoord>{});
		}
		OutTriangulation[CoordA].Add(CoordB);
		if (!OutTriangulation.Contains(CoordB))
		{
			OutTriangulation.Add(CoordB, TSet<FTileCoord>{});
		}
		OutTriangulation[CoordB].Add(CoordA);
	}

	return OutTriangulation;
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
		if (Head.Value.Num() == PathLengthInNodes + 1) // Checks +1 because the FTileCoord(0,0) counts as a node so path length will be one less than the counted nodes
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
	for (float PathingNoiseExponent = 10; PathingNoiseExponent >= 0; PathingNoiseExponent -= 1)
	{
		TMap<FTileCoord, FTileCoord> Path;
		Path.Add(FTileCoord(), FTileCoord());

		bool HasFailed = false;
		// For every node in the path array, use A* to connect it to the previous node
		for (int NodeIndex = 0; NodeIndex < PathNodes.Num()-1; NodeIndex++)
		{
			const FTileCoord StartTile = PathNodes[NodeIndex];
			const FTileCoord EndTile = PathNodes[NodeIndex+1];

			TMap<FTileCoord, FTileCoord> PathFromEndToStart;

			// Get set of tiles in existing path. These are used to exclude the path from searches, and to avoid tiles adjacent to the path
			TSet<FTileCoord> ExcludedPath = {FTileCoord()};
			for (TTuple<FTileCoord, FTileCoord> Element : Path)
			{
				// Add a tile in the path to ExcludedTiles
				ExcludedPath.Add(Element.Value);
			}
			TSet<FTileCoord> ExcludedFutureNodes;
			for (FTileCoord Element : PathNodes)
			{
				// Also exclude each future path node
				ExcludedFutureNodes.Add(Element);
			}
			
			if (AStarPathFind(EndTile, StartTile, PathingNoiseOffset, ExcludedPath, ExcludedFutureNodes, PathFromEndToStart, PathingNoiseExponent))
			{
				Path.Append(PathFromEndToStart);
			} else
			{
				HasFailed = true;
				break;
			}
		}
		if (!HasFailed)
		{
			return Path;
		}
		UE_LOG(LogTemp, Warning, TEXT("UAegisMapFactory::GenerateAStarPathThroughNodes - Failed with Exponent of %f, trying again with lower exponent..."), PathingNoiseExponent);
	}
	
	UE_LOG(LogTemp, Error, TEXT("UAegisMapFactory::GenerateAStarPathThroughNodes - Could not find path through nodes!!"));
	return TMap<FTileCoord, FTileCoord>();	
}

bool UAegisMapFactory::AStarPathFind(const FTileCoord StartTile, const FTileCoord GoalTile, FVector2D PathingNoiseOffset, const TSet<FTileCoord>& ExcludedPath, const TSet<FTileCoord>& ExcludedFutureNodes, TMap<FTileCoord, FTileCoord>& OutputPath, float WeightExponent)
{	
	if (WeightExponent < 0)
	{
		UE_LOG(LogTemp, Error, TEXT("UAegisMapFactory::AStarPathFind - Weight multiplier cannot be below 0! Cancelling A* Search."));
		return false;
	}

	TSet<FTileCoord> ExcludedPathAdjacent;
	for (FTileCoord Element : ExcludedPath)
	{
		// Find nodes adjacent to the excluded tiles to avoid but not necessarily be excluded from traversing
		ExcludedPathAdjacent.Append(FTileCoord::GetTilesInRadius(Element, 1));
	}

	
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
			// Found the goal! reconstruct the path and return
			FTileCoord TraceBackHead = Current;
			while (TraceBackHead != StartTile)
			{
				FTileCoord TraceBackParent = CameFrom[TraceBackHead];
				OutputPath.Add(TraceBackParent, TraceBackHead);
				TraceBackHead = TraceBackParent;
			}
			return true;
		}

		for (FTileCoord Neighbour : FTileCoord::GetTilesInRadius(Current, 1))
		{
			// If the tile is in the path, or part of a future node, skip it
			if ((ExcludedPath.Contains(Neighbour) || ExcludedFutureNodes.Contains(Neighbour)) && Neighbour != GoalTile) { continue; }
			
			// Find the weight of the neighbor tile. Fetched using the pathing noise distribution. Low Perlin scale to be more "random".
			float NextWeight = GetNodeWeight(Neighbour, PathingNoiseOffset, true, WeightExponent);

			// If the tile is adjacent to the path, increase the weight to encourage a spacing. (but will go through it if there is no other option)
			if (ExcludedPathAdjacent.Contains(Neighbour))// && !FTileCoord::GetTilesInRadius(GoalTile, 1).Contains(Neighbour)
			{
				NextWeight += 200;
			}

			const float NewCost = CostSoFar[Current] + NextWeight;
			if (!CostSoFar.Contains(Neighbour) || NewCost < CostSoFar[Neighbour])
			{
				// This path to the neighbour is better than any previous one, so record it
				CameFrom.Add(Neighbour, Current);
				CostSoFar.Add(Neighbour, NewCost);
				float Heuristic = FVector::Distance(GoalTileLocation, Neighbour.ToWorldLocation()) / 100.f;
				float Priority = NewCost + Heuristic;
				Frontier.Push(Neighbour, Priority);
			}
		}
	}
	
	UE_LOG(LogTemp, Error, TEXT("UAegisMapFactory::AStarPathFind - Could not find A* path! returning false..."));
	return false;
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

	const float WaterLimit = ElevationNoiseDistribution[static_cast<int>((ElevationNoiseDistribution.Num() - 1) * 0.15)];
	const float Grass1Limit = ElevationNoiseDistribution[static_cast<int>((ElevationNoiseDistribution.Num() - 1) * 0.7)];
	const float Grass2Limit = ElevationNoiseDistribution[static_cast<int>((ElevationNoiseDistribution.Num() - 1) * 0.85)];
	const float Grass3Limit = ElevationNoiseDistribution[static_cast<int>((ElevationNoiseDistribution.Num() - 1) * 0.90)];
	const float Grass4Limit = ElevationNoiseDistribution[static_cast<int>((ElevationNoiseDistribution.Num() - 1) * 0.95)];
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

		if (Elem.Value->ElevationNoise < Grass1Limit)
		{
			Elem.Value->Elevation = 0;
		}
		else if (Elem.Value->ElevationNoise < Grass2Limit)
		{
			Elem.Value->Elevation = 1;
		}
		else if (Elem.Value->ElevationNoise < Grass3Limit)
		{
			Elem.Value->Elevation = 2;
		}
		else if (Elem.Value->ElevationNoise < Grass4Limit)
		{
			Elem.Value->Elevation = 3;
		}
		else
		{
			Elem.Value->Elevation = 4;
		}

		// Set max elevation (and adjust terrain type if necessary) based on distance to the start and end path tiles
		//const int MaxElevation = FTileCoord::HexDistance(FTileCoord(), Elem.Key);
		const int MaxElevation = FTileCoord::HexDistanceToTiles(PathTiles, Elem.Key);
		Elem.Value->Elevation = FMath::Min(Elem.Value->Elevation, MaxElevation);

		switch (Elem.Value->Elevation)
		{
		case 0:
			Elem.Value->TerrainType = Grass;
			break;
		case 1:
			Elem.Value->TerrainType = Grass;
			break;
		case 2:
			Elem.Value->TerrainType = Grass;
			break;
		case 3:
			Elem.Value->TerrainType = Grass;
			break;
		case 4:
			Elem.Value->TerrainType = Cliff;
			break;
		default:
			break;
		}
		
		if (Elem.Value->ElevationNoise < WaterLimit)
		{
			Elem.Value->Elevation = 0;
			Elem.Value->TerrainType = Water;
		}

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

	// Clamp the value to be between 0->1 to avoid negative weights?
	const float x = (FMath::PerlinNoise2D(NoiseSampleLoc) + 1.f) / 2.f;

	// If used for pathing, the noise is multiplied exponentially to make high weights worth more 
	if (bDistortNoise)
	{
		return FMath::Pow(x + 0.5, 10.f);
	}

	return x;
}
