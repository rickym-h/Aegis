// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Structures/Towers/TowerData.h"
#include "ProjectileTowerData.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class AEGIS_API UProjectileTowerData : public UTowerData
{
	GENERATED_BODY()

public:
	UProjectileTowerData();

	virtual ATower* SpawnTowerFromData(FVector LocationToSpawnTower) const override;
};
