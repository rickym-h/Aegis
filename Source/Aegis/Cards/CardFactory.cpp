// Fill out your copyright notice in the Description page of Project Settings.


#include "CardFactory.h"

#include "PlayerCard.h"

TArray<UPlayerCard*> UCardFactory::GenerateStarterTowers(UObject* OuterGameInstance) const
{
	TArray<UPlayerCard*> OutCards;

	for (const TSubclassOf<UPlayerCard> StarterCardClass : DefaultCards)
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
	case God:
		CardClassArray.Append(GodCards);
		break;
	case Valen:
		CardClassArray.Append(ValenCards);
		break;
	case Caius:
		CardClassArray.Append(CaiusCards);
		break;
	default:
		UE_LOG(LogTemp, Error, TEXT("UCardFactory::GenerateStarterTowers - Character Enum not recognised! Creating default starter cards."));
		break;
	}
	CardClassArray.Append(DefaultCards);
	

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
