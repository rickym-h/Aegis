// Fill out your copyright notice in the Description page of Project Settings.


#include "AegisPlayerController.h"

#include "ResourcesData.h"
#include "Aegis/AegisGameInstance.h"
#include "Aegis/AegisGameStateBase.h"
#include "Aegis/Cards/PlayableCard.h"
#include "Aegis/Cards/PlayerCard.h"
#include "Aegis/Enemies/EnemyFactory.h"
#include "Aegis/Map/TileCoordHelperLibrary.h"
#include "Kismet/GameplayStatics.h"

AAegisPlayerController::AAegisPlayerController()
{
	Resources = CreateDefaultSubobject<UResourcesData>("Resources");
}

void AAegisPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Get cards from the game instance
	
	if (const UAegisGameInstance* GameInstance = Cast<UAegisGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		DrawPile.Append(GameInstance->PlayerDeck);	
	}
	
	DiscardAndReplenishHand();

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

bool AAegisPlayerController::SelectCard(UPlayerCard* PlayerCard)
{
	UE_LOG(LogTemp, Display, TEXT("AAegisPlayerController::SelectCard - Selecting card!"))
	if (!CardsInHand.Contains(PlayerCard)) { return false; }
	
	SelectedCard = PlayerCard;
	return true;
}

UPlayerCard* AAegisPlayerController::GetSelectedCard() const
{
	return SelectedCard.Get();
}

void AAegisPlayerController::TryPlayCardAtLocation(UPlayerCard* Card, const FTileCoord& LocationCoord) const
{
	// Check the player has enough resources
	if (!Resources->IsResourcesEnough(Card->CardCost))
	{
		UE_LOG(LogTemp, Error, TEXT("AAegisPlayerController::TryPlayCardAtLocation - Player does not have enough resources to play card!"))
		return;
	}

	// Check if the card implements the IPlayableCard interface
	if (!UKismetSystemLibrary::DoesImplementInterface(Card, UPlayableCard::StaticClass()))
	{
		UE_LOG(LogTemp, Error, TEXT("AAegisPlayerController::TryPlayCardAtLocation - Card does not implement UPlayableCard interface!"))
		return;
	}
	
	if (!IPlayableCard::Execute_PlayCard(Card, LocationCoord))
	{
		UE_LOG(LogTemp, Error, TEXT("AAegisPlayerController::TryPlayCardAtLocation - Attempted to play card but failed. Check implemented PlayCard_Implementation()"))
		return;
	}
	
	Resources->SpendResources(Card->CardCost);
	UE_LOG(LogTemp, Display, TEXT("AAegisPlayerController::TryPlayCardAtLocation - Played card successfully! Spent resources!"))
}

void AAegisPlayerController::ClickGround()
{
	FHitResult HitResultUnderCursor;
	GetHitResultUnderCursor(ECC_Visibility, true, HitResultUnderCursor);
	
	const FTileCoord LocationCoord = FTileCoord::FromWorldLocation(HitResultUnderCursor.Location);
	
	// Attempt to play the selected card, if any
	if (UPlayerCard* Card = SelectedCard.Get())
	{
		TryPlayCardAtLocation(Card, LocationCoord);
		return;
	}

	// TODO select a building or tile on ground
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
		if (!DrawCards(1, false))
		{
			break;
		}
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

