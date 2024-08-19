// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AegisGameInstance.generated.h"

class UPlayerCard;
/**
 * 
 */
UCLASS()
class AEGIS_API UAegisGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void OnStart() override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player Data")
	TArray<UPlayerCard*> PlayerDeck;

protected:
};
