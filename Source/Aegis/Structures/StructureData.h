// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Map/TileCoordHelperLibrary.h"
#include "Aegis/Map/MapTiles/MapTileData.h"
#include "StructureData.generated.h"


class UAegisMap;

UENUM()
enum EStructureType
{
	Resources UMETA(DisplayName = "Resources"),
	Research_Progression UMETA(DisplayName = "Research & Progression"),
	Amenities_Services UMETA(DisplayName = "Amenities & Services"),
	ContractTower UMETA(DisplayName = "Contract Tower"),
	PhysicalTower UMETA(DisplayName = "Contract Tower"),
	MagicTower UMETA(DisplayName = "Contract Tower"),
	SupportTower UMETA(DisplayName = "Contract Tower"),
};

class AStructure;
/**
 * 
 */
UCLASS()
class AEGIS_API UStructureData : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Tower Class")
	TSubclassOf<AStructure> StructureBlueprintClass;

	/**
	 * This is the mesh shown when trying to place the structure. Ideally a hologram of some form
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Structure Data")
	UStaticMesh* MeshRepresentation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MetaData")
	UAegisMap* MapReference;

public:
	UFUNCTION()
	UStaticMesh* GetMeshRepresentation() const;

	UFUNCTION()
	TSubclassOf<AStructure> GetStructureBlueprintClass() const;

	UFUNCTION(BlueprintCallable)
	virtual AStructure* SpawnStructureFromData(const FTileCoord CoordToSpawnStructure, const FVector BuildingLocation) const
	{
		UE_LOG(LogTemp, Fatal,
		       TEXT("UStructureData::SpawnStructureFromData() - Base structure data function called - should be overridden by leaf classes."))
		return nullptr;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tower Data")
	FString StructureName = "Default Name";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tower Data")
	bool bRemoveInstanceOnPlacement = false;

	// A list of terrain types this structure can be placed on
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tower Data")
	TArray<TEnumAsByte<ETerrainType>> AllowedTerrains;
	// A list of resource types this structure can be placed on
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tower Data")
	TArray<TEnumAsByte<EResourceType>> AllowedResources;

	UFUNCTION()
	bool IsTileTypeValid(FTileCoord Location);
	
	UFUNCTION(BlueprintCallable)
	virtual bool CanStructureBePlaced(FTileCoord Location);
};
