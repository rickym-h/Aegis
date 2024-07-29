// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Structures/Buildings/BuildingData.h"
#include "ResourceCollectorData.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class AEGIS_API UResourceCollectorData : public UBuildingData
{
	GENERATED_BODY()

public:
	virtual AStructure* SpawnStructureFromData(const FTileCoord CoordToSpawnStructure, const FVector BuildingLocation, UStructureData* StructureData) const override;
};
