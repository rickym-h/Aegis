// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/WorldMap/WorldMapBlueprintFunctionLibrary.h"
#include "UObject/Object.h"
#include "PlayerData.generated.h"

class UResourcesData;

UENUM(BlueprintType)
enum EPlayerCharacter
{
	God = 0,
	Caius = 1,
	Valen = 2,
};

class UPlayerCard;
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class AEGIS_API UPlayerData : public UObject
{
	GENERATED_BODY()

	UPlayerData();

public:
	UFUNCTION(BlueprintCallable, Category = "PlayerData")
	FName GetPlayerName() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerData")
	TEnumAsByte<EPlayerCharacter> GetPlayerCharacter() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerData")
	void SetPlayerCharacter(const TEnumAsByte<EPlayerCharacter>& InPlayerCharacter);

	UFUNCTION(BlueprintCallable, Category = "PlayerData")
	TArray<UPlayerCard*> GetPlayerDeck() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerData")
	UResourcesData* GetPlayerResources() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerData")
	void SetPlayerDeck(const TArray<UPlayerCard*>& InPlayerDeck);

	UFUNCTION(BlueprintCallable, Category = "PlayerData")
	FWorldMapData GetWorldMapData() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerData")
	void SetWorldMapData(const FWorldMapData& InWorldMapData);

private:	
	UPROPERTY(EditAnywhere, Category="Player Data")
	TEnumAsByte<EPlayerCharacter> PlayerCharacter = EPlayerCharacter::God;

	UPROPERTY(EditAnywhere, Category="Player Data")
	TArray<UPlayerCard*> PlayerDeck = TArray<UPlayerCard*>();

	UPROPERTY(EditAnywhere, Category="Player Data")
	FWorldMapData WorldMapData = FWorldMapData();

	UPROPERTY(EditAnywhere, Category="Player Data")
	UResourcesData* ResourcesData;
};
