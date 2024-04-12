// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TowerDataFactory.generated.h"

class UStructureData;
class ATower;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class AEGIS_API UTowerDataFactory : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Towers")
	TSubclassOf<UStructureData> TestTowerDataClass;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="World")
	UWorld* WorldReference;

	UFUNCTION(BlueprintCallable)
	UStructureData* GenerateTestTowerData() const;
};
