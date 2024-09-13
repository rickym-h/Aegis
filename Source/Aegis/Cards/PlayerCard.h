// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Pawns/ResourcesData.h"
#include "UObject/NoExportTypes.h"
#include "PlayerCard.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, HideDropdown)
class AEGIS_API UPlayerCard : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Card Data")
	FText CardName = FText::FromString("Default Card Name");
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Card Data")
	FText CardDescription = FText::FromString("Default Card Description");
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Card Data")
	FResources CardCost = FResources();
};
