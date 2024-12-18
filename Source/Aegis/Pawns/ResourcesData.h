// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ResourcesData.generated.h"

/**
 * 
*/
USTRUCT(BlueprintType, Blueprintable)
struct FResources
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resources")
	int32 Wood = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resources")
	int32 Stone = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resources")
	int32 Gold = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resources")
	int32 Runes = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resources")
	int32 ManaCrystals = 0;

	FResources operator*(const int32 Multiplier) const
	{
		FResources R;
		R.Wood = Wood * Multiplier;
		R.Stone = Stone * Multiplier;
		R.Gold = Gold * Multiplier;
		R.Runes = Runes * Multiplier;
		R.ManaCrystals = ManaCrystals * Multiplier;
		return R;
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnResourcesUpdatedSignature);

UCLASS()
class AEGIS_API UResourcesData : public UObject
{
	GENERATED_BODY()

public:
	void InitResources(const FResources& InResources);
	
	UPROPERTY(BlueprintAssignable)
	FOnResourcesUpdatedSignature OnResourcesUpdatedDelegate;
	
	UFUNCTION(BlueprintCallable)
	void AddResources(const int32 InWood, const int32 InStone, const int32 InGold, const int32 InRunes, const int32 InManaCrystals);
	void AddResources(const UResourcesData* ResourcesToAdd);
	void AddResources(const FResources& ResourcesToAdd);
	UFUNCTION(BlueprintCallable)
	bool IsResourcesEnough(const FResources& ResourcesCost) const;
	UFUNCTION(BlueprintCallable)
	void SpendResources(const FResources& Cost);

	UFUNCTION(BlueprintCallable)
	FResources GetResources() const;
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
	FResources Resources;
};
