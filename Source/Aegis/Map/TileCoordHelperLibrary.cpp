// Fill out your copyright notice in the Description page of Project Settings.


#include "TileCoordHelperLibrary.h"

FTileCoord UTileCoordHelperLibrary::MakeTileCoordStruct(int Q, int R)
{
	return FTileCoord(Q, R);
}

TArray<FTileCoord> UTileCoordHelperLibrary::RotateTileCoords(const TArray<FTileCoord>& Coords, const bool bClockwise)
{
	//https://www.redblobgames.com/grids/hexagons/#rotation
	
	TArray<FTileCoord> RotatedCoords;
	RotatedCoords.Reserve(Coords.Num());

	for (const FTileCoord& Coord : Coords)
	{
		if (bClockwise)
		{
			// Clockwise rotation: [q,r,s] -> [-r,-s,-q]
			RotatedCoords.Add(FTileCoord(-Coord.R, -(-(Coord.Q) - Coord.R)));
		}
		else
		{
			// Counter-clockwise rotation: [q,r,s] -> [-s,-q,-r]
			RotatedCoords.Add(FTileCoord(-(-(Coord.Q) - Coord.R), -Coord.Q));
		}
	}

	return RotatedCoords;
}

TSet<FTileCoord> UTileCoordHelperLibrary::RotateTileCoords(const TSet<FTileCoord>& Coords, const bool bClockwise)
{
	return TSet(RotateTileCoords(Coords.Array(), bClockwise));
}
