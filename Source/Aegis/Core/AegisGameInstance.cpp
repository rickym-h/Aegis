// Fill out your copyright notice in the Description page of Project Settings.


#include "AegisGameInstance.h"

#include "Aegis/Cards/CardFactory.h"

void UAegisGameInstance::OnStart()
{
	Super::OnStart();
	
	if (!CardFactoryClass)
	{
		UE_LOG(LogTemp, Error, TEXT("UAegisGameInstance::OnStart - Card factory value not set!"))
		return;
	}
	CardFactory = NewObject<UCardFactory>(this, CardFactoryClass);
}

void UAegisGameInstance::SetPlayerData(const TArray<UPlayerCard*> InPlayerDeck, const FWorldMapData InWorldMapData, const FString InPlayerName)
{
	this->PlayerName = InPlayerName;
	this->PlayerDeck = InPlayerDeck;
	this->WorldMapData = InWorldMapData;
}
