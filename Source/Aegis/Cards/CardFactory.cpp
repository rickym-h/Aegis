// Fill out your copyright notice in the Description page of Project Settings.


#include "CardFactory.h"

#include "PlayerCard.h"

TArray<UPlayerCard*> UCardFactory::GenerateStarterTowersForCharacter(UObject* OuterGameInstance, const TEnumAsByte<EPlayerCharacter> Character) const
{
	TArray<UPlayerCard*> OutCards;
	if (!OuterGameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("UCardFactory::GenerateStarterTowers - Encountered a null OuterGameInstance"))
		return OutCards;
	}
	
	// Load the card lists from the data tables
	TArray<FCardData*> CardData;
	const FString ContextString = "Loading Enemy Data...";
	switch (Character)
	{
	case God:
		DefaultKamiCards->GetAllRows<FCardData>(ContextString, CardData);
		break;
	case Valen:
		DefaultValenCards->GetAllRows<FCardData>(ContextString, CardData);
		break;
	case Caius:
		DefaultCaiusCards->GetAllRows<FCardData>(ContextString, CardData);
		break;
	default:
		UE_LOG(LogTemp, Error, TEXT("UCardFactory::GenerateStarterTowers - Character Enum not recognised! Creating default starter cards."));
		break;
	}

	// Get each class from the data table rows
	TArray<TSubclassOf<UPlayerCard>> CardClassArray;
	for (const FCardData* Card : CardData)
	{
		CardClassArray.Add(Card->CardClass);
	}

	// Create instances of each class to return
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
