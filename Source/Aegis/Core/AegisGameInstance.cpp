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

UPlayerData* UAegisGameInstance::GetPlayerData() const
{
	return PlayerData;
}

void UAegisGameInstance::SetPlayerData(UPlayerData* InPlayerData)
{
	this->PlayerData = InPlayerData;

	OnPlayerDataUpdated.Broadcast(PlayerData);
}
