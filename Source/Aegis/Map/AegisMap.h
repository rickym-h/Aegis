// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TileCoordHelperLibrary.h"
#include "AegisMap.generated.h"


class UMapTileData;
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
	void DestroyMap();

	
	UFUNCTION()
	void PopulateMapData(const TMap<FTileCoord, UMapTileData*>& InMapTileData, const TMap<FTileCoord, FTileCoord>& InPathRoute,
	                     const TArray<FTileCoord>& InPathStartTiles, ANexusBuilding* InNexusBuilding);

	UFUNCTION()
	bool IsCoordInPath(FTileCoord Coord) const;

	UFUNCTION()
	AMapTile* GetTile(FTileCoord Coord);
	UFUNCTION()
	TArray<AMapTile*> GetTiles();

	UFUNCTION()
	FTileCoord GetEnemySpawnCoord() const;
	UFUNCTION()
	FTileCoord GetNextCoordInPath(const FTileCoord CurrentCoord) const;

	UFUNCTION(BlueprintCallable)
	bool AddStructureToMap(const FTileCoord Location, UStructureData* StructureData);

	UFUNCTION(BlueprintCallable)
	int GetNumOfTilesToEnd(const FTileCoord StartCoord);

	UPROPERTY()
	ANexusBuilding* NexusBuilding;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Blueprints")
	TSubclassOf<AMapTile> MapTileBP;

protected:
	// Map Tiles
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Map Data")
	TMap<FTileCoord, AMapTile*> MapTiles;
	
	AMapTile* CreateMapTile(const FTileCoord Coord, UMapTileData* MapTileData) const;
	TMap<FTileCoord, AMapTile*> GenerateMapTiles(const TMap<FTileCoord, UMapTileData*>& MapTileData) const;

	// Path Data
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Map Data")
	TMap<FTileCoord, FTileCoord> PathRoute;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Map Data")
	TArray<FTileCoord> PathStartTiles;

	// Structure Data
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Map Data")
	TMap<FTileCoord, AStructure*> MapStructures;

	UFUNCTION(BlueprintCallable)
	bool IsTileAvailable(const FTileCoord Location) const;
};
