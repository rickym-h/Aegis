// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Cards/StructureCard.h"
#include "Aegis/Cards/Interfaces/PlayableCardInterface.h"
#include "Aegis/Cards/Interfaces/TileRangeInterface.h"
#include "LaserTowerCard.generated.h"

/**
 * 
 */
UCLASS(HideDropDown)
class AEGIS_API ULaserTowerCard : public UStructureCard, public IPlayableCardInterface, public ITileRangeInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower Details")
	float DamagePerSecond = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower Details")
	float LaseFireRate = 10.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Card Data")
	int32 RangeTiles = 0;
	
protected:
	virtual bool PlayCard_Implementation(const FTileCoord& LocationCoord) override;
	virtual TSet<FTileCoord> GetRangeTileOffsets_Implementation() override;
	virtual TSet<FTileCoord> RotateTileRangeOffsets_Implementation(const bool bClockwise) override;
};
