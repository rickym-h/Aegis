// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Clickable.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UClickable : public UInterface
{
	GENERATED_BODY()
};

/**
 * An interface for actors that can be clicked in the world.
 * E.g. Towers to see statsm, or enemies for more details.
 */
class AEGIS_API IClickable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interface Functions")
	void Clicked();
	virtual void Clicked_Implementation();
};
