// Fill out your copyright notice in the Description page of Project Settings.


#include "AegisPlayerController.h"

#include "ResourcesData.h"
#include "Aegis/AegisGameStateBase.h"
#include "Aegis/Enemies/EnemyFactory.h"

AAegisPlayerController::AAegisPlayerController()
{
	Resources = CreateDefaultSubobject<UResourcesData>("Resources");
}

void AAegisPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// If there are no cards in the draw pile, give the player some starter towers
	// TODO make the deck persistent in the Game Instance
	
	// if (const UAegisGameInstance* GameInstance = Cast<UAegisGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	// {
	// 	DrawPile.Append(GameInstance->PlayerDeck);
	// }
	
	//DiscardAndReplenishHand();

	const AAegisGameStateBase* GameState = Cast<AAegisGameStateBase>(GetWorld()->GetGameState());
	GameState->EnemyFactory->OnWaveEndDelegate.AddUniqueDynamic(this, &AAegisPlayerController::DiscardAndReplenishHand);
}

void AAegisPlayerController::ShuffleDrawPile()
{
	DrawPile.Sort([this](const UPlayerCard &Card1, const UPlayerCard &Card2)
	{
		return FMath::RandBool();
	});
}

void AAegisPlayerController::ClickGround()
{
	UE_LOG(LogTemp, Warning, TEXT("AAegisPlayerController::ClickGround() - Click detected!"))
}

TArray<UPlayerCard*> AAegisPlayerController::GetCardsInHand()
{
	return CardsInHand;
}

bool AAegisPlayerController::Discard(UPlayerCard* CardToDiscard)
{
	if (!CardsInHand.Contains(CardToDiscard))
	{
		UE_LOG(LogTemp, Error, TEXT("AAegisPlayerController::Discard - Card not in player hand, cannot discard!"))
		return false;
	}

	CardsInHand.RemoveSingle(CardToDiscard);
	DiscardPile.Add(CardToDiscard);

	OnCardsInHandUpdatedDelegate.Broadcast();

	return true;
}

void AAegisPlayerController::DiscardAllCards()
{
	DiscardPile.Append(CardsInHand);
	CardsInHand = TArray<UPlayerCard*>();

	OnCardsInHandUpdatedDelegate.Broadcast();
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
	
	OnCardsInHandUpdatedDelegate.Broadcast();
	
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
			DiscardPile = TArray<UPlayerCard*>();

			ShuffleDrawPile();
		}

		// Draw a single card
		UPlayerCard* Card = DrawPile.Pop();
		CardsInHand.Add(Card);
	}

	if (BroadcastUpdate)
	{
		OnCardsInHandUpdatedDelegate.Broadcast();
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

