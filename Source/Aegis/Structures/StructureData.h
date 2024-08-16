// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Map/TileCoordHelperLibrary.h"
#include "Aegis/Map/MapTiles/MapTileData.h"
#include "Aegis/Pawns/ResourcesData.h"
#include "StructureData.generated.h"


class UAegisMap;
class AStructure;
/**
 * 
 */
UCLASS(BlueprintType)
class AEGIS_API UStructureData : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Structure Data")
	FResources StructureCost;

	UFUNCTION()
	TSubclassOf<AStructure> GetStructureBlueprintClass() const;

	UFUNCTION(BlueprintCallable)
	virtual AStructure* SpawnStructureFromData(const FTileCoord CoordToSpawnStructure, const FVector BuildingLocation, UStructureData* StructureData) const
	{
		UE_LOG(LogTemp, Fatal,
		       TEXT("UStructureData::SpawnStructureFromData() - Base structure data function called - should be overridden by leaf classes."))
		return nullptr;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Structure Data")
	FString StructureName = "Default Name";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Structure Data")
	bool bRemoveInstanceOnPlacement = false;

	// A list of terrain types this structure can be placed on
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Structure Data")
	TArray<TEnumAsByte<ETerrainType>> AllowedTerrains;
	// A list of resource types this structure can be placed on
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Structure Data")
	TArray<TEnumAsByte<EResourceType>> AllowedResources;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Structure Data")
	TArray<FTileCoord> StructureOffsets;

	
	UFUNCTION(BlueprintCallable)
	virtual bool CanStructureBePlaced(const FTileCoord Location, const bool bCheckSingleTile = false);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Tower Class")
	TSubclassOf<AStructure> StructureBlueprintClass;
	
	UPROPERTY(BlueprintReadOnly, Category="MetaData")
	UAegisMap* MapReference;
	
	UFUNCTION()
	bool IsTileTypeValid(FTileCoord Location) const;

};
