// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Structures/Towers/TowerData.h"
#include "Aegis/Utilities/ProjectileManager.h"
#include "ProjectileTowerData.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class AEGIS_API UProjectileTowerData : public UTowerData
{
	GENERATED_BODY()

public:
	virtual AStructure* SpawnStructureFromData(const FTileCoord CoordToSpawnStructure, const FVector BuildingLocation) const override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile Data")
	UStaticMesh* ProjectileMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile Tower Data")
	int TowerRange = 2;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile Data")
	float ProjectileSpeed = 20;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile Data")
	float AttackSpeed = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile Data")
	FProjectileDamagePackage DamagePackage;
};
