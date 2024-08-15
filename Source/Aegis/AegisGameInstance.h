// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AegisGameInstance.generated.h"

class UStructureData;
class UStructureDataFactory;
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
	TArray<UStructureData*> PlayerDeck;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Factories")
	UStructureDataFactory* StructureDataFactory;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Factory Classes")
	TSubclassOf<UStructureDataFactory> StructureDataFactoryClass;
};
