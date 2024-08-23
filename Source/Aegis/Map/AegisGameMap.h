// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AegisMap.h"
#include "AegisGameMap.generated.h"

/**
 * 
 */
UCLASS()
class AEGIS_API UAegisGameMap : public UAegisMap
{
	GENERATED_BODY()

public:
	UFUNCTION()
	FTileCoord GetEnemySpawnCoord() const;

	UFUNCTION()
	FTileCoord GetNextCoordInPath(const FTileCoord CurrentCoord) const;

	UFUNCTION(BlueprintCallable)
	int GetNumOfTilesToEnd(const FTileCoord StartCoord);

	virtual bool IsTileAvailable(const FTileCoord& Location) const override;

	
	void PopulateGameMapData(const TMap<FTileCoord, UMapTileData*>& InMapTileData, const TMap<FTileCoord, FTileCoord>& InPath, const TArray<FTileCoord>& InPathStartCoords, ANexusBuilding* InNexusBuilding);


	UPROPERTY()
	ANexusBuilding* NexusBuilding;
protected:
	
	
	// Path Data
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Map Data")
	TMap<FTileCoord, FTileCoord> PathRoute;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Map Data")
	TArray<FTileCoord> PathStartTiles;
	
};
