// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Map/TileCoordHelperLibrary.h"
#include "UObject/Interface.h"
#include "PlayableCardInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayableCardInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AEGIS_API IPlayableCardInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interface Functions")
	bool PlayCard(const FTileCoord& LocationCoord);
	virtual bool PlayCard_Implementation(const FTileCoord& LocationCoord) {return false;};
};
