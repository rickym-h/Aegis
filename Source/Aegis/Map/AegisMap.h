// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TileCoordHelperLibrary.h"
#include "AegisMap.generated.h"


class UStructureData;
class AStructure;
class UTowerData;
class ABaseTower;
class ANexusBuilding;
class AMapTile;

/**
 * 
 */
UCLASS(Blueprintable)
class AEGIS_API UAegisMap : public UObject
{
	GENERATED_BODY()

public:
	UAegisMap();

	UFUNCTION()
	void PopulateMapData(const TMap<FTileCoord, AMapTile*>& InMapTiles, const TMap<FTileCoord, FTileCoord>& InPathRoute, const TArray<FTileCoord>& InPathStartTiles, ANexusBuilding* InNexusBuilding);
	
	UFUNCTION()
	bool IsCoordInPath(FTileCoord Coord) const;

	UFUNCTION()
	AMapTile* GetTile(FTileCoord Coord);

	UFUNCTION()
	FTileCoord GetEnemySpawnCoord() const;
	UFUNCTION()
	FTileCoord GetNextCoordInPath(const FTileCoord CurrentCoord) const;

	UFUNCTION(BlueprintCallable)
	bool AddStructureToMap(const FTileCoord Location, UStructureData* StructureData);

	UFUNCTION(BlueprintCallable)
	int GetNumOfTilesToEnd(const FTileCoord StartCoord);

protected:
	// Map Tiles
	TMap<FTileCoord, AMapTile*> MapTiles;

	// Path Data
	TMap<FTileCoord, FTileCoord> PathRoute;
	TArray<FTileCoord> PathStartTiles;

	// Structure Data
	UPROPERTY()
	ANexusBuilding* NexusBuilding;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Map Data")
	TMap<FTileCoord, AStructure*> MapStructures;

	UFUNCTION(BlueprintCallable)
	bool IsTileAvailable(const FTileCoord Location) const;
	
};
