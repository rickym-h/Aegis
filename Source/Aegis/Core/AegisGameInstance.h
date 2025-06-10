// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AegisGameInstance.generated.h"

class UPlayerData;
class UCardFactory;
class UPlayerCard;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerDataUpdatedSignature, UPlayerData*, PlayerData);
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
	UPlayerData* GetPlayerData() const;

	UFUNCTION(BlueprintCallable)
	void SetPlayerData(UPlayerData* InPlayerData);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Factories")
	UCardFactory* CardFactory;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerDataUpdatedSignature OnPlayerDataUpdated;

protected:
	
	UPROPERTY(EditDefaultsOnly, Category="Factories")
	TSubclassOf<UCardFactory> CardFactoryClass;

	UPROPERTY(EditAnywhere, Category="Player Data")
	UPlayerData* PlayerData;
};
