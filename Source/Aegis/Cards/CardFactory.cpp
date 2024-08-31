// Fill out your copyright notice in the Description page of Project Settings.


#include "CardFactory.h"

#include "PlayerCard.h"

TArray<UPlayerCard*> UCardFactory::GenerateStarterTowers(UObject* OuterGameInstance) const
{
	TArray<UPlayerCard*> OutCards;

	for (const TSubclassOf<UPlayerCard> StarterCardClass : StarterCards)
	{
		if (!StarterCardClass)
		{
			UE_LOG(LogTemp, Error, TEXT("UCardFactory::GenerateStarterTowers - Encountered a null StarterCard"))
			return OutCards;
		}
		if (!OuterGameInstance)
		{
			UE_LOG(LogTemp, Error, TEXT("UCardFactory::GenerateStarterTowers - Encountered a null OuterGameInstance"))
			return OutCards;
		}

		UPlayerCard* StructureData = NewObject<UPlayerCard>(OuterGameInstance, StarterCardClass);
		
		OutCards.Add(StructureData);
	}

	return OutCards;
}
