// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerData.h"

FName UPlayerData::GetPlayerName() const
{
	return PlayerName;
}

void UPlayerData::SetPlayerName(const FName& InPlayerName)
{
	this->PlayerName = InPlayerName;
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
