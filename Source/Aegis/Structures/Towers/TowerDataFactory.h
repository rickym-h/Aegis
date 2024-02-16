// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TowerDataFactory.generated.h"

class UTowerData;
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
	TSubclassOf<UTowerData> TestTowerDataClass;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="World")
	UWorld* WorldReference;
	
	UFUNCTION(BlueprintCallable)
	UTowerData* GenerateTestTowerData() const;
};
