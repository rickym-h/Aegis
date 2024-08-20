// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Cards/PlayableCard.h"
#include "Aegis/Cards/StructureCard.h"
#include "ArcherCard.generated.h"

/**
 * 
 */
UCLASS()
class AEGIS_API UArcherCard : public UStructureCard, public IPlayableCard
{
	GENERATED_BODY()

protected:
	virtual bool PlayCard_Implementation(const FTileCoord& LocationCoord) override;

public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Archer Card Data")
	float RangeInMetres = 5.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Archer Card Data")
	float ShotsPerSecond = 0.5f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Archer Card Data")
	float DamagePerShot = 1.f;
};
