// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Cards/StructureCard.h"
#include "Aegis/Cards/Interfaces/PlayableCardInterface.h"
#include "Aegis/Cards/Interfaces/RangeInterface.h"
#include "Aegis/Cards/Interfaces/TileRangeInterface.h"
#include "Aegis/Utilities/ProjectileManager.h"
#include "ProjectileTowerCard.generated.h"

/**
 * 
 */
UCLASS(HideDropDown)
class AEGIS_API UProjectileTowerCard : public UStructureCard, public IPlayableCardInterface, public ITileRangeInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Tower Card Data")
	FProjectileDamagePackage ProjectileDamagePackage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Tower Card Data")
	float AttackSpeed = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Tower Card Data")
	int32 RangeTiles = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Tower Card Data")
	UStaticMesh* ProjectileMesh;
	

protected:
	virtual bool PlayCard_Implementation(const FTileCoord& LocationCoord) override;
	virtual TSet<FTileCoord> GetRangeTileOffsets_Implementation() override;
};
