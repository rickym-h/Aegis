// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Map/TileCoordHelperLibrary.h"
#include "UObject/Interface.h"
#include "TileRangeInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UTileRangeInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AEGIS_API ITileRangeInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interface Functions")
	TSet<FTileCoord> GetRangeTileOffsets();
	virtual TSet<FTileCoord> GetRangeTileOffsets_Implementation() { return TSet<FTileCoord>(); };

	// TODO rotatetile range offsets 
};
