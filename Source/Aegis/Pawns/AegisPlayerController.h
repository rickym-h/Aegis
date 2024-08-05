// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AegisPlayerController.generated.h"

class UResourcesData;
class UStructureData;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTowersInHandUpdated);

/**
 * 
 */
UCLASS()
class AEGIS_API AAegisPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AAegisPlayerController();

protected:

	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cards")
	TArray<UStructureData*> DrawPile;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cards")
	TArray<UStructureData*> CardsInHand;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cards")
	TArray<UStructureData*> DiscardPile;

	UFUNCTION()
	void ShuffleDrawPile();
	
public:
	
	/**
	 * 
	 * @return A list of cards currently 'playable' to be used in game. A subset of cards in the player's deck.
	 */
	UFUNCTION(BlueprintCallable)
	TArray<UStructureData*> GetCardsInHand();
	
	/**
	 * 
	 * @param CardToDiscard A pointer to a card in the player's hand to 'play'. This will then be put into the discard pile to later be reshuffled into the draw pile.
	 * @return A bool representing whether the card has been discarded successfully or not.
	 */
	UFUNCTION(BlueprintCallable)
	bool Discard(UStructureData* CardToDiscard);

	/**
	 * Moves all cards in the current hand into the DiscardPile
	 */
	UFUNCTION(BlueprintCallable)
	void DiscardAllCards();

	/**
	 * Adds random cards from the DrawPile into the CardsInHand. Shuffles the DiscardPile into the DrawPile if necessary.
	 * @param HandTargetCount A target number of cards to have in the players hand at the end. (could result in less if there are not enough cards in the player deck)
	 * @return A bool representing whether the target count has been reached or not.
	 */
	UFUNCTION(BlueprintCallable)
	bool ReplenishHand(const int32 HandTargetCount);
	
	UPROPERTY(BlueprintAssignable)
	FOnTowersInHandUpdated OnTowersInHandUpdatedDelegate;

	UFUNCTION(BlueprintCallable)
	int32 GetDrawPileCount() const;
	UFUNCTION(BlueprintCallable)
	int32 GetDiscardPileCount() const;
	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Resources")
	UResourcesData* Resources;
};
