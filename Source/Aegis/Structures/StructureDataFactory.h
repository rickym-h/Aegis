// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StructureDataFactory.generated.h"

class UStructureData;
class ATower;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class AEGIS_API UStructureDataFactory : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Towers")
	TSubclassOf<UStructureData> TestTowerDataClass;
	UFUNCTION(BlueprintCallable)
	UStructureData* GenerateTestTowerData(UWorld* WorldRef) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Towers")
	TSubclassOf<UStructureData> LumberYardDataClass;
	UFUNCTION(BlueprintCallable)
	UStructureData* GenerateLumberYardData(UWorld* WorldRef) const;



public:
	UFUNCTION(BlueprintCallable)
	TArray<UStructureData*> GenerateStarterTowers(UWorld* WorldRef) const;
};
