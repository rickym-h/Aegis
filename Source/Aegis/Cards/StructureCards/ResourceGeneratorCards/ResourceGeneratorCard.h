// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Cards/PlayableCardInterface.h"
#include "Aegis/Cards/StructureCard.h"
#include "ResourceGeneratorCard.generated.h"

/**
 * 
 */
UCLASS()
class AEGIS_API UResourceGeneratorCard : public UStructureCard, public IPlayableCardInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LumberYard Data")
	FResources WaveEndGeneratedResources;
	
protected:
	virtual bool PlayCard_Implementation(const FTileCoord& LocationCoord) override;
};
