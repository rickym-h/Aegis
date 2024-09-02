// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/WorldMap/WorldMapBlueprintFunctionLibrary.h"
#include "UObject/Object.h"
#include "PlayerData.generated.h"

UENUM(BlueprintType)
enum EPlayerCharacter
{
	Custom = 0,
	Darius = 1,
};

class UPlayerCard;
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class AEGIS_API UPlayerData : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
	FName GetPlayerName() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerData")
	void SetPlayerName(const FName& InPlayerName);

	UFUNCTION(BlueprintCallable, Category = "PlayerData")
	TEnumAsByte<EPlayerCharacter> GetPlayerCharacter() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerData")
	void SetPlayerCharacter(const TEnumAsByte<EPlayerCharacter>& InPlayerCharacter);

	UFUNCTION(BlueprintCallable, Category = "PlayerData")
	TArray<UPlayerCard*> GetPlayerDeck() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerData")
	void SetPlayerDeck(const TArray<UPlayerCard*>& InPlayerDeck);

	UFUNCTION(BlueprintCallable, Category = "PlayerData")
	FWorldMapData GetWorldMapData() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerData")
	void SetWorldMapData(const FWorldMapData& InWorldMapData);

private:
	UPROPERTY(EditAnywhere, Category="Player Data")
	FName PlayerName = "";
	
	UPROPERTY(EditAnywhere, Category="Player Data")
	TEnumAsByte<EPlayerCharacter> PlayerCharacter = Custom;

	UPROPERTY(EditAnywhere, Category="Player Data")
	TArray<UPlayerCard*> PlayerDeck = TArray<UPlayerCard*>();

	UPROPERTY(EditAnywhere, Category="Player Data")
	FWorldMapData WorldMapData = FWorldMapData();

};
