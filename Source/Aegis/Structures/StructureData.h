// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Map/TileCoordHelperLibrary.h"
#include "StructureData.generated.h"

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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Structure Data")
	UStaticMesh* MeshRepresentation;

public:
	UFUNCTION()
	UStaticMesh* GetMeshRepresentation() const;

	UFUNCTION()
	TSubclassOf<AStructure> GetStructureBlueprintClass() const;

	UFUNCTION(BlueprintCallable)
	virtual AStructure* SpawnStructureFromData(const FTileCoord CoordToSpawnStructure, const FVector BuildingLocation) const
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("UStructureData::SpawnStructureFromData() - Base structure data function called - should be overridden by leaf classes."))
		return nullptr;
	}
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Tower Data")
	FString StructureName = "Default Name";
};
