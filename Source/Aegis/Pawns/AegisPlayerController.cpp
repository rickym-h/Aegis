// Fill out your copyright notice in the Description page of Project Settings.


#include "AegisPlayerController.h"

#include "ResourcesData.h"
#include "Aegis/AegisGameStateBase.h"
#include "Aegis/Enemies/EnemyFactory.h"
#include "Aegis/Structures/StructureData.h"
#include "Aegis/Structures/StructureDataFactory.h"

AAegisPlayerController::AAegisPlayerController()
{
	Resources = CreateDefaultSubobject<UResourcesData>("Resources");
}

void AAegisPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// If there are no cards in the draw pile, give the player some starter towers
	// TODO make the deck persistent in the Game Instance
	if (DrawPile.Num() == 0)
	{
		if (const AAegisGameStateBase* GameState = Cast<AAegisGameStateBase>(GetWorld()->GetGameState()))
		{
			DrawPile.Append(GameState->StructureDataFactory->GenerateStarterTowers(GetWorld()));
		}
	}
	DiscardAndReplenishHand();

	AAegisGameStateBase* GameState = Cast<AAegisGameStateBase>(GetWorld()->GetGameState());
	GameState->EnemyFactory->OnWaveEndDelegate.AddUniqueDynamic(this, &AAegisPlayerController::DiscardAndReplenishHand);
}

void AAegisPlayerController::ShuffleDrawPile()
{
	DrawPile.Sort([this](const UStructureData &Card1, const UStructureData &Card2)
	{
		return FMath::RandBool();
	});
}

TArray<UStructureData*> AAegisPlayerController::GetCardsInHand()
{
	return CardsInHand;
}

bool AAegisPlayerController::Discard(UStructureData* CardToDiscard)
{
	if (!CardsInHand.Contains(CardToDiscard))
	{
		UE_LOG(LogTemp, Error, TEXT("AAegisPlayerController::Discard - Card not in player hand, cannot discard!"))
		return false;
	}

	CardsInHand.RemoveSingle(CardToDiscard);
	DiscardPile.Add(CardToDiscard);

	OnTowersInHandUpdatedDelegate.Broadcast();

	return true;
}

void AAegisPlayerController::DiscardAllCards()
{
	DiscardPile.Append(CardsInHand);
	CardsInHand = TArray<UStructureData*>();

	OnTowersInHandUpdatedDelegate.Broadcast();
}

void AAegisPlayerController::DiscardAndReplenishHand()
{
	DiscardAllCards();
	ReplenishHand(3);
}


bool AAegisPlayerController::ReplenishHand(const int32 HandTargetCount)
{
	while (CardsInHand.Num() < HandTargetCount)
	{
		DrawCards(1, false);
	}
	
	OnTowersInHandUpdatedDelegate.Broadcast();
	
	return HandTargetCount == CardsInHand.Num();
}

bool AAegisPlayerController::DrawCards(const int32 NumOfCards, const bool BroadcastUpdate)
{
	const int32 CardsInHandCount = CardsInHand.Num();
	
	for (int i = 0; i < NumOfCards; i++)
	{
		// If needed, shuffle in the discard pile
		if (DrawPile.Num() == 0)
		{
			// Shuffle in the discard pile

			if (DiscardPile.Num() == 0)
			{
				// There are no cards in the discard pile to shuffle in
				break;
			}
			
			DrawPile.Append(DiscardPile);
			DiscardPile = TArray<UStructureData*>();

			ShuffleDrawPile();
		}

		// Draw a single card
		UStructureData* Card = DrawPile.Pop();
		CardsInHand.Add(Card);
	}

	if (BroadcastUpdate)
	{
		OnTowersInHandUpdatedDelegate.Broadcast();
	}

	return CardsInHandCount + NumOfCards == CardsInHand.Num();
}

int32 AAegisPlayerController::GetDrawPileCount() const
{
	return DrawPile.Num();
}

int32 AAegisPlayerController::GetDiscardPileCount() const
{
	return DiscardPile.Num();
}

