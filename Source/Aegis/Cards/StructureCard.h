// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCard.h"
#include "Aegis/Map/MapTiles/MapTileData.h"
#include "StructureCard.generated.h"

class AStructure;
/**
 * 
 */
UCLASS()
class AEGIS_API UStructureCard : public UPlayerCard
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Structure Data")
	TArray<TEnumAsByte<ETerrainType>> AllowedTerrains = {Grass};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Structure Data")
	TArray<TEnumAsByte<EResourceType>> AllowedResources = {Empty};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Tower Class")
	TSubclassOf<AStructure> StructureBlueprintClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Structure Data")
	TArray<FTileCoord> StructureOffsets;
};
