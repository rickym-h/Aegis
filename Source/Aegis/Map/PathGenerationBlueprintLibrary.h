// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TileCoordHelperLibrary.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PathGenerationBlueprintLibrary.generated.h"

struct FTileCoord;
/**
 * 
 */
UCLASS()
class AEGIS_API UPathGenerationBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Path Generation")
	static TArray<FTileCoord> GetClustersInRange(const int MapRadius);
	
	UFUNCTION(BlueprintCallable, Category="Path Generation")
	static TArray<FTileCoord> GetAdjacentClusters(const FTileCoord Coord);
	
};
