// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Map/TileCoordHelperLibrary.h"
#include "MapTileData.generated.h"

UENUM()
enum ETerrainType
{
	Water UMETA(DisplayName = "Water"),
	Grass UMETA(DisplayName = "Grass"),
	Sand UMETA(DisplayName = "Sand"),
	Rock UMETA(DisplayName = "Rock"),
	Cliff UMETA(DisplayName = "Cliff"),
};

UENUM()
enum EResourceType
{
	Empty UMETA(DisplayName = "Empty"),
	Tree UMETA(DisplayName = "Tree"),
	Stone UMETA(DisplayName = "Stone"),
	TreeStone UMETA(DisplayName = "Tree & Stone"),
};

/**
 * 
 */
UCLASS()
class AEGIS_API UMapTileData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Data")
	TEnumAsByte<ETerrainType> TerrainType = Grass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Data")
	bool bIsPath = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Data")
	TEnumAsByte<EResourceType> ResourceType = Empty;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Data")
	float TreeNoise = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Data")
	float StoneNoise = 0;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Data")
	int32 Elevation = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Data")
	float ElevationNoise = 0;
};
