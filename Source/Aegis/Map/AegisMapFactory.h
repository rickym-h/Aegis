// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AegisMap.h"
#include "AegisMapFactory.generated.h"

class UAegisGameMap;
class UAegisWorldMap;
class ANexusBuilding;
class AMapTile;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class AEGIS_API UAegisMapFactory : public UObject
{
	GENERATED_BODY()

public:
	virtual void PostInitProperties() override;

	static TArray<FTileCoord> GetPathStartCoords(TMap<FTileCoord, FTileCoord> PathRoute);

	UFUNCTION()
	UAegisGameMap* GenerateGameMap(const int PathNodeLength) const;
	UFUNCTION()
	UAegisGameMap* GenerateWorldMap(const int PathNodeLength) const;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Map")
	TSubclassOf<UAegisGameMap> AegisGameMapClass;
	UPROPERTY(EditDefaultsOnly, Category="Map")
	TSubclassOf<UAegisWorldMap> WorldMapClass;

	UPROPERTY(EditDefaultsOnly, Category="Tiles")
	TSubclassOf<AMapTile> GrassTileBP;
	
	UPROPERTY(EditDefaultsOnly, Category="Tiles")
	TSubclassOf<ANexusBuilding> NexusBuildingBP;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Tiles")
	int TileRadius = 100;

	UPROPERTY(BlueprintReadOnly, Category="Tiles")
	float TileHeight;
	UPROPERTY(BlueprintReadOnly, Category="Tiles")
	float TileWidth;
	UPROPERTY(BlueprintReadOnly, Category="Tiles")
	float VerticalSpacing;
	UPROPERTY(BlueprintReadOnly, Category="Tiles")
	float HorizontalSpacing;

	UPROPERTY(BlueprintReadOnly, Category="Tiles")
	FVector OffsetQ;
	UPROPERTY(BlueprintReadOnly, Category="Tiles")
	FVector OffsetR;
};
