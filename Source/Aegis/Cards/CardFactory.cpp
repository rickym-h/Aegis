// Fill out your copyright notice in the Description page of Project Settings.


#include "CardFactory.h"

#include "InputBehavior.h"
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

TArray<UPlayerCard*> UCardFactory::GenerateStarterTowersForCharacter(UObject* OuterGameInstance, const TEnumAsByte<EPlayerCharacter> Character) const
{
	TArray<UPlayerCard*> OutCards;
	if (!OuterGameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("UCardFactory::GenerateStarterTowers - Encountered a null OuterGameInstance"))
		return OutCards;
	}
	
	TArray<TSubclassOf<UPlayerCard>> CardClassArray;
	switch (Character)
	{
	case Darius:
		CardClassArray.Append(DariusStarterCards);
		break;
	default:
		UE_LOG(LogTemp, Error, TEXT("UCardFactory::GenerateStarterTowers - Character Enum not recognised! Creating default starter towers."));
		CardClassArray.Append(StarterCards);
		break;
	}
	

	for (const TSubclassOf<UPlayerCard> StarterCardClass : CardClassArray)
	{
		if (!StarterCardClass)
		{
			UE_LOG(LogTemp, Error, TEXT("UCardFactory::GenerateStarterTowers - Encountered a null StarterCard"))
			return OutCards;
		}

		UPlayerCard* StructureData = NewObject<UPlayerCard>(OuterGameInstance, StarterCardClass);
		
		OutCards.Add(StructureData);
	}

	return OutCards;
}
