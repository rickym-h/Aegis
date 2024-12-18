// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerData.h"

FName UPlayerData::GetPlayerName() const
{
	switch(PlayerCharacter)
	{
	case God: return FName("God");
	case Caius: return FName("Caius");
	case Valen: return FName("Valen");
	default: return FName("No Name Found");
	}
}

TEnumAsByte<EPlayerCharacter> UPlayerData::GetPlayerCharacter() const
{
	return PlayerCharacter;
}

void UPlayerData::SetPlayerCharacter(const TEnumAsByte<EPlayerCharacter>& InPlayerCharacter)
{
	this->PlayerCharacter = InPlayerCharacter;
}

TArray<UPlayerCard*> UPlayerData::GetPlayerDeck() const
{
	return PlayerDeck;
}

UResourcesData* UPlayerData::GetPlayerResources() const
{
	return ResourcesData;
}

void UPlayerData::SetPlayerDeck(const TArray<UPlayerCard*>& InPlayerDeck)
{
	this->PlayerDeck = InPlayerDeck;
}

FWorldMapData UPlayerData::GetWorldMapData() const
{
	return WorldMapData;
}

void UPlayerData::SetWorldMapData(const FWorldMapData& InWorldMapData)
{
	this->WorldMapData = InWorldMapData;
}
