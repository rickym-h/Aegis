// Fill out your copyright notice in the Description page of Project Settings.


#include "MapTileData.h"

UMapTileData::UMapTileData()
{
	TerrainType = Grass;
	bIsPath = false;
	ResourceType = Empty;
	Elevation = 1;
	ElevationNoise = 0;
}
