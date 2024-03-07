// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "AegisSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class AEGIS_API UAegisSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	
	UPROPERTY(VisibleAnywhere, Category = Basic)
	uint32 NexusHealth;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	FString SaveSlotName;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	uint32 UserIndex;
	
	UAegisSaveGame();
};
