// Fill out your copyright notice in the Description page of Project Settings.


#include "AegisMapFactory.h"

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
	return TArray<FTileCoord>();
}

UAegisGameMap* UAegisMapFactory::GenerateGameMap(const int PathNodeLength) const
{
	return nullptr;
}

UAegisGameMap* UAegisMapFactory::GenerateWorldMap(const int PathNodeLength) const
{
	return nullptr;
}
