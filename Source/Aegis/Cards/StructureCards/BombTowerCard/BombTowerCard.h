// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Cards/PlayableCardInterface.h"
#include "Aegis/Cards/RangeInterface.h"
#include "Aegis/Cards/StructureCard.h"
#include "BombTowerCard.generated.h"

/**
 * 
 */
UCLASS()
class AEGIS_API UBombTowerCard : public UStructureCard, public IPlayableCardInterface, public IRangeInterface
{
	GENERATED_BODY()

protected:
	virtual bool PlayCard_Implementation(const FTileCoord& LocationCoord) override;
	virtual float GetRangeInMetres_Implementation() override;

public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Archer Card Data")
	float RangeInMetres = 5.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Archer Card Data")
	float ShotsPerSecond = 0.5f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Archer Card Data")
	float DamagePerShot = 1.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Archer Card Data")
	float ExplosionRadius = 100.f;
	
};
