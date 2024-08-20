// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CardFactory.generated.h"

class UAegisGameInstance;
class UPlayerCard;
/**
 * 
 */
UCLASS(Blueprintable)
class AEGIS_API UCardFactory : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	TArray<UPlayerCard*> GenerateStarterTowers(UObject* OuterGameInstance) const;
	
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cards")
	TArray<TSubclassOf<UPlayerCard>> StarterCards;
};
