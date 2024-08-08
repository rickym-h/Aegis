// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Structures/Towers/TowerData.h"
#include "ArcherTowerData.generated.h"

/**
 * 
 */
UCLASS()
class AEGIS_API UArcherTowerData : public UTowerData
{
	GENERATED_BODY()

public:
	virtual AStructure* SpawnStructureFromData(const FTileCoord CoordToSpawnStructure, const FVector BuildingLocation, UStructureData* StructureData) const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Archer Tower Data")
	float RangeInMetres;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Archer Tower Data")
	float ShotsPerSecond;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Archer Tower Data")
	float Damage;
};
