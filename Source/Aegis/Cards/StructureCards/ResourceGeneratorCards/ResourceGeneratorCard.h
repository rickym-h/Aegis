// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Cards/StructureCard.h"
#include "Aegis/Cards/Interfaces/PlayableCardInterface.h"
#include "ResourceGeneratorCard.generated.h"

/**
 * 
 */
UCLASS(HideDropDown)
class AEGIS_API UResourceGeneratorCard : public UStructureCard, public IPlayableCardInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ResourceGenerator Data")
	FResources WaveEndGeneratedResources;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ResourceGenerator Data")
	TMap<TEnumAsByte<EResourceType>, FResources> ResourcesToGeneratePerAdjacentResource;
	
protected:
	virtual bool PlayCard_Implementation(const FTileCoord& LocationCoord) override;
};
