// Fill out your copyright notice in the Description page of Project Settings.


#include "PathGenerationBlueprintLibrary.h"

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
