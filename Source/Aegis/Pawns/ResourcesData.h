// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ResourcesData.generated.h"

/**
 * 
*/

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnResourcesUpdatedSignature);

UCLASS()
class AEGIS_API UResourcesData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnResourcesUpdatedSignature OnResourcesUpdatedDelegate;
	
	UFUNCTION(BlueprintCallable)
	void AddResources(const int32 InWood, const int32 InStone, const int32 InGold, const int32 InRunes, const int32 InManaCrystals);

	UFUNCTION(BlueprintCallable)
	int32 GetWood() const;
	UFUNCTION(BlueprintCallable)
	int32 GetStone() const;
	UFUNCTION(BlueprintCallable)
	int32 GetGold() const;
	UFUNCTION(BlueprintCallable)
	int32 GetRunes() const;
	UFUNCTION(BlueprintCallable)
	int32 GetManaCrystals() const;

protected:
	UPROPERTY(VisibleAnywhere, Category="Resources")
	int32 Wood = 0;
	UPROPERTY(VisibleAnywhere, Category="Resources")
	int32 Stone = 0;
	UPROPERTY(VisibleAnywhere, Category="Resources")
	int32 Gold = 0;
	UPROPERTY(VisibleAnywhere, Category="Resources")
	int32 Runes = 0;
	UPROPERTY(VisibleAnywhere, Category="Resources")
	int32 ManaCrystals = 0;
};
