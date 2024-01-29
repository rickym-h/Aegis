// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AegisMap.h"
#include "Aegis/AegisGameStateBase.h"
#include "AegisMapFactory.generated.h"

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
	
	UFUNCTION()
	UAegisMap* GenerateTestMap() const;

protected:

	UPROPERTY(EditDefaultsOnly, Category="Map")
	TSubclassOf<UAegisMap> AegisMapClass;
	
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
