// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldMapBlueprintFunctionLibrary.h"

FWorldMapData UWorldMapBlueprintFunctionLibrary::GenerateWorldMapData(const FWorldMapCreationConfig MapConfig)
{
	FWorldMapData WorldMapData = FWorldMapData();
	WorldMapData.InitialConfig = MapConfig;

	// Create a Start and Boss node.
	const FMapNodeCoordinate StartNodeCoord = FMapNodeCoordinate(0, static_cast<int32>(MapConfig.GridRows / 2));
	FMapNode StartNode = FMapNode(MinorEnemy, StartNodeCoord);
	WorldMapData.MapNodes.Add(StartNode);

	const FMapNodeCoordinate BossNodeCoord = FMapNodeCoordinate(MapConfig.LayersCount+1, static_cast<int32>(MapConfig.GridRows / 2));
	FMapNode BossNode = FMapNode(BossEnemy, BossNodeCoord);
	WorldMapData.MapNodes.Add(BossNode);
	
	// Ensure layer depth is greater than width??
	if (MapConfig.LayersCount < MapConfig.GridRows)
	{
		UE_LOG(LogTemp, Error, TEXT("UWorldMapBlueprintFunctionLibrary::GenerateWorldMapData() - Layer count is shorter than grid width. Risks unreachable paths."))
		return WorldMapData;
	}
	// Ensure layer counts are valid
	if (MapConfig.StartingLayerNodesCount > MapConfig.GridRows || MapConfig.PreBossLayerNodesCount > MapConfig.GridRows)
	{
		UE_LOG(LogTemp, Error, TEXT("UWorldMapBlueprintFunctionLibrary::GenerateWorldMapData() - Starting or pre-boss node counts are greater than grid width!"))
		return WorldMapData;
	}

	UE_LOG(LogTemp, Display, TEXT("UWorldMapBlueprintFunctionLibrary::GenerateWorldMapData() - Starting selection of path starts and ends. "))
		
	// Select some ending and starting nodes.
	TArray<int32> CandidateRows;
	for (int i = 0; i < MapConfig.GridRows; i++)
		CandidateRows.Add(i);
	CandidateRows.Sort([](const int32 A, const int32 B)
	{
		return FMath::RandBool();
	});
	TArray<int32> StartingRows;
	for (int i = 0; i < MapConfig.StartingLayerNodesCount; i++)
	{
		StartingRows.Add(CandidateRows[i]);
	}
	CandidateRows.Sort([](const int32 A, const int32 B)
	{
		return FMath::RandBool();
	});
	TArray<int32> EndNodeRows;
	for (int i = 0; i < MapConfig.PreBossLayerNodesCount; i++)
	{
		EndNodeRows.Add(CandidateRows[i]);
	}

	UE_LOG(LogTemp, Display, TEXT("UWorldMapBlueprintFunctionLibrary::GenerateWorldMapData() - Generating Paths! "))
	
	// Generate paths of count = start & end nodes up to the number of max(numOfStart, numOfEnd) + MapConfig.NumOfExtraPaths
	const int NumOfPaths = FMath::Max(MapConfig.StartingLayerNodesCount, MapConfig.PreBossLayerNodesCount) + FMath::Max(0, MapConfig.NumOfExtraPaths);
	for (int i = 0; i < NumOfPaths; i++)
	{
		UE_LOG(LogTemp, Display, TEXT("UWorldMapBlueprintFunctionLibrary::GenerateWorldMapData() - Generating Path: %i "), i)
		const int32 PathStartNodeRow = StartingRows[i % MapConfig.StartingLayerNodesCount];
		const FMapNodeCoordinate PathStartNodeCoord = FMapNodeCoordinate(1, PathStartNodeRow);
		const int32 PathEndNodeRow = EndNodeRows[i % MapConfig.PreBossLayerNodesCount];
		const FMapNodeCoordinate PathEndNodeCoord = FMapNodeCoordinate(MapConfig.LayersCount, PathEndNodeRow);

		// For each pair, generate a path between the start and end
		GeneratePath(WorldMapData, PathStartNodeCoord, PathEndNodeCoord);
		
		// Add a path between start node and this path start node
		WorldMapData.MapNodes[WorldMapData.GetMapNodeIndex(StartNodeCoord)].OutGoingConnections.Add(PathStartNodeCoord);
		
		// Add a path between final node and boss node
		WorldMapData.MapNodes[WorldMapData.GetMapNodeIndex(PathEndNodeCoord)].OutGoingConnections.Add(BossNodeCoord);
	}

	
	UE_LOG(LogTemp, Display, TEXT("UWorldMapBlueprintFunctionLibrary::GenerateWorldMapData() - Resolving crosspaths... "))

	// Iterate over every x4 set and resolve crosspaths
	ResolveCrossPaths(WorldMapData);

	WorldMapData.HeadNode = FMapNode();
	WorldMapData.HeadNode.OutGoingConnections = {StartNodeCoord};
	
	return WorldMapData;
}

FVector2D UWorldMapBlueprintFunctionLibrary::GetMapPosition(const FMapNodeCoordinate NodeCoordinate, const float ContainerHeight, const int32 RowCount)
{
	const float NodeSpacing = ContainerHeight / (RowCount+1);

	const float xPos = (NodeCoordinate.Layer+1) * NodeSpacing;
	const float yPos = (NodeCoordinate.Row+1) * NodeSpacing;

	return FVector2D(xPos, yPos);
}

FWorldMapData UWorldMapBlueprintFunctionLibrary::TravelToNode(const FMapNode Node, FWorldMapData WorldMapData)
{
	if (!WorldMapData.MapNodes.Contains(Node))
	{
		UE_LOG(LogTemp, Error, TEXT("UWorldMapBlueprintFunctionLibrary::TravelToNode - Node is not in WorldMapData.MapNodes"));
		return WorldMapData;
	}

	if (!WorldMapData.HeadNode.OutGoingConnections.Contains(Node.NodeCoordinate))
	{
		UE_LOG(LogTemp, Error, TEXT("UWorldMapBlueprintFunctionLibrary::TravelToNode - Node coordinate is not a valid explorable coordinate from the head node."));
		return WorldMapData;
	}
	
	WorldMapData.HeadNode = Node;

	return WorldMapData;
}

FWorldMapData UWorldMapBlueprintFunctionLibrary::ExploreHeadNode(FWorldMapData WorldMapData)
{
	WorldMapData.ExploredNodes.Add(WorldMapData.HeadNode);
	return WorldMapData;
}

float UWorldMapBlueprintFunctionLibrary::GetAngleBetweenTwoPoints(const FVector2D Origin, const FVector2D Destination)
{
	return FMath::Atan2(Destination.Y - Origin.Y, Destination.X - Origin.X);
}

FWorldMapData UWorldMapBlueprintFunctionLibrary::CreatePrototypeWorldMapData()
{
	// Simple static map for now
	FWorldMapData WorldMapData;

	FMapNode StartingNode = FMapNode(MinorEnemy, FMapNodeCoordinate(0, 0));
	
	FMapNode MiddleEnemy = FMapNode(MinorEnemy, FMapNodeCoordinate(1, 0));
	StartingNode.OutGoingConnections.Add(FMapNodeCoordinate(1, 0));
	
	FMapNode MiddleShop = FMapNode(Shop, FMapNodeCoordinate(1, 1));
	StartingNode.OutGoingConnections.Add(FMapNodeCoordinate(1, 1));
	
	FMapNode MiddleRest = FMapNode(RestSite, FMapNodeCoordinate(2, 0));
	MiddleEnemy.OutGoingConnections.Add(FMapNodeCoordinate(2, 0));
	MiddleShop.OutGoingConnections.Add(FMapNodeCoordinate(2, 0));
	
	FMapNode EndingNode = FMapNode(BossEnemy, FMapNodeCoordinate(3, 1));
	MiddleRest.OutGoingConnections.Add(FMapNodeCoordinate(3, 1));

	WorldMapData.MapNodes.Add(StartingNode);
	WorldMapData.MapNodes.Add(MiddleEnemy);
	WorldMapData.MapNodes.Add(MiddleShop);
	WorldMapData.MapNodes.Add(MiddleRest);
	WorldMapData.MapNodes.Add(EndingNode);

	WorldMapData.HeadNode = FMapNode();
	WorldMapData.HeadNode.OutGoingConnections = {StartingNode.NodeCoordinate};

	return WorldMapData;
}

void UWorldMapBlueprintFunctionLibrary::GeneratePath(FWorldMapData& WorldMapData, const FMapNodeCoordinate StartNodeCoordinate, const FMapNodeCoordinate EndNodeCoordinate)
{
	// Create node if not exists at start node
	if (WorldMapData.GetMapNodeIndex(StartNodeCoordinate) == -1)
	{
		UE_LOG(LogTemp, Display, TEXT("UWorldMapBlueprintFunctionLibrary::GeneratePath - Start Node doesnt exit, creating a new one. "))
		FMapNode PathStartNode = FMapNode(MinorEnemy, StartNodeCoordinate);
		WorldMapData.MapNodes.Add(PathStartNode);
	}

	// Set the Head as a coordinate at the start node
	FMapNodeCoordinate HeadCoord = StartNodeCoordinate;
	UE_LOG(LogTemp, Display, TEXT("UWorldMapBlueprintFunctionLibrary::GeneratePath - Set Head Coord. "))
	
	// While until head node is at the end node
	while (HeadCoord != EndNodeCoordinate)
	{
		// Get a set of possible nodes the next head could be at
		TArray<FMapNodeCoordinate> CandidateHeads;
		for (int i = -1; i <= 1; i++)
		{
			FMapNodeCoordinate PossibleHead = FMapNodeCoordinate(HeadCoord.Layer+1, HeadCoord.Row+i);

			const int32 LayerDistance = FMath::Abs(EndNodeCoordinate.Layer - PossibleHead.Layer);
			const int32 RowDistance = FMath::Abs(EndNodeCoordinate.Row - PossibleHead.Row);

			if (PossibleHead.Row < 0 || PossibleHead.Row >= WorldMapData.InitialConfig.GridRows) { continue; }
			if (RowDistance > LayerDistance) { continue; }

			CandidateHeads.Add(PossibleHead);
		}
		UE_LOG(LogTemp, Display, TEXT("UWorldMapBlueprintFunctionLibrary::GeneratePath - Set candidate heads. Count: %i"), CandidateHeads.Num())
		// Select a random one of the possible nodes, and place a new node there.
		const int32 RandomIndex = FMath::RandRange(0, CandidateHeads.Num() - 1);
		FMapNodeCoordinate NewHeadCoord = CandidateHeads[RandomIndex];

		// Create new node at new head coord if it does not exist
		if (WorldMapData.GetMapNodeIndex(NewHeadCoord) == -1)
		{
			FMapNode NewNode = FMapNode(EMapNodeType::MinorEnemy, NewHeadCoord);
			WorldMapData.MapNodes.Add(NewNode);
		}

		WorldMapData.MapNodes[WorldMapData.GetMapNodeIndex(HeadCoord)].OutGoingConnections.Add(NewHeadCoord);

		// Set the head to be the new created node coord
		HeadCoord = NewHeadCoord;
	}
	return;
}

void UWorldMapBlueprintFunctionLibrary::ResolveCrossPaths(const FWorldMapData& WorldMapData)
{
	// TODO
}
