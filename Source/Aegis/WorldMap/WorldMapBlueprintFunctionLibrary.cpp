// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldMapBlueprintFunctionLibrary.h"

FWorldMapData UWorldMapBlueprintFunctionLibrary::GenerateWorldMapData(const FWorldMapCreationConfig MapConfig)
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

	return WorldMapData;
}

FVector2D UWorldMapBlueprintFunctionLibrary::GetMapPosition(const FMapNodeCoordinate NodeCoordinate, const float ContainerHeight, const int32 RowCount)
{
	const float NodeSpacing = ContainerHeight / (RowCount+1);

	const float xPos = (NodeCoordinate.Layer+1) * NodeSpacing;
	const float yPos = (NodeCoordinate.Row+1) * NodeSpacing;

	return FVector2D(xPos, yPos);
}

FWorldMapData UWorldMapBlueprintFunctionLibrary::ExploreNode(const FMapNode Node, FWorldMapData WorldMapData)
{
	if (!WorldMapData.MapNodes.Contains(Node))
	{
		UE_LOG(LogTemp, Error, TEXT("UWorldMapBlueprintFunctionLibrary::ExploreNode - Node is not in WorldMapData.MapNodes"));
		return WorldMapData;
	}

	if (!WorldMapData.HeadNode.OutGoingConnections.Contains(Node.NodeCoordinate))
	{
		UE_LOG(LogTemp, Error, TEXT("UWorldMapBlueprintFunctionLibrary::ExploreNode - Node coordinate is not a valid explorable coordinate from the head node."));
		return WorldMapData;
	}
	
	WorldMapData.ExploredNodes.Add(Node);
	WorldMapData.HeadNode = Node;

	return WorldMapData;
}

float UWorldMapBlueprintFunctionLibrary::GetAngleBetweenTwoPoints(const FVector2D Origin, const FVector2D Destination)
{
	return FMath::Atan2(Destination.Y - Origin.Y, Destination.X - Origin.X);
}
