// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Structures/StructureData.h"
#include "UObject/NoExportTypes.h"
#include "TowerData.generated.h"

class ATower;
/**
 * 
 */
UCLASS(Blueprintable)
class AEGIS_API UTowerData : public UStructureData
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Tower Class")
	TSubclassOf<ATower> TowerBlueprintClass;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Tower Data")
	FString DefenderName = "Default Name";

	UFUNCTION()
	TSubclassOf<ATower> GetTowerBlueprintClass() const;

	UFUNCTION(BlueprintCallable)
	virtual ATower* SpawnTowerFromData(FVector LocationToSpawnTower) const
	{
		UE_LOG(LogTemp, Warning, TEXT("UTowerData::SpawnTowerFromData() - Base tower data function called - should be overridden by leaf classes."))
		return nullptr;
	}
};
