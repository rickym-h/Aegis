// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Cards/StructureCard.h"
#include "Aegis/Cards/Interfaces/PlayableCardInterface.h"
#include "Aegis/Cards/Interfaces/TileRangeInterface.h"
#include "TileThumperCard.generated.h"

/**
 * 
 */
UCLASS(HideDropDown)
class AEGIS_API UTileThumperCard : public UStructureCard, public IPlayableCardInterface, public ITileRangeInterface
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Tower Card Data")
	float AttackSpeed = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Tower Card Data")
	TSet<FTileCoord> TileRangeOffsets = TSet<FTileCoord>();
	

protected:
	virtual bool PlayCard_Implementation(const FTileCoord& LocationCoord) override;
	virtual TSet<FTileCoord> GetRangeTileOffsets_Implementation() override;
};
