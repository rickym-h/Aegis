// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TileCoordHelperLibrary.h"
#include "AegisMap.generated.h"


class UStructureCard;
class AStructure;
class AMapTile;
class ANexusBuilding;
class UMapTileData;
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
	AMapTile* GetTile(FTileCoord Coord);
	UFUNCTION()
	TArray<AMapTile*> GetTiles();
	UFUNCTION()
	UMapTileData* GetTileData(FTileCoord Coord);

	AStructure* AddStructureToMap(const UStructureCard* StructureCard, const FTileCoord Location, bool bFinishSpawningStructure = false);
	
	UFUNCTION(BlueprintCallable)
	virtual bool IsTileAvailable(const FTileCoord& Location) const;
	bool CanStructureBePlaced(const UStructureCard* StructureCard, const FTileCoord& Location);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Blueprints")
	TSubclassOf<AMapTile> MapTileBP;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Map Data")
	TMap<FTileCoord, AMapTile*> MapTiles;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Map Data")
	TMap<FTileCoord, UMapTileData*> MapTileDataMap;

	AMapTile* CreateMapTile(const FTileCoord Coord, UMapTileData* MapTileData) const;
	TMap<FTileCoord, AMapTile*> GenerateMapTilesFromData() const;


	// Structure Data
	TMap<FTileCoord, UStructureCard*> MapStructureData;
	TMap<FTileCoord, AStructure*> MapStructures;

};
