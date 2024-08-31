// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Utilities/CircleMathBlueprintLibrary.h"
#include "Aegis/WorldMap/WorldMapBlueprintFunctionLibrary.h"
#include "Engine/GameInstance.h"
#include "AegisGameInstance.generated.h"

class UCardFactory;
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

	UFUNCTION(BlueprintCallable)
	void SetPlayerData(TArray<UPlayerCard*> InPlayerDeck, FWorldMapData InWorldMapData, FString InPlayerName);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player Data")
	FString PlayerName = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player Data")
	TArray<UPlayerCard*> PlayerDeck;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Player Data")
	FWorldMapData WorldMapData;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Factories")
	UCardFactory* CardFactory;
	
protected:
	
	UPROPERTY(EditDefaultsOnly, Category="Factories")
	TSubclassOf<UCardFactory> CardFactoryClass;
};
