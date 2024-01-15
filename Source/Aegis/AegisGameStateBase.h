// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "AegisGameStateBase.generated.h"

class UAegisMapFactory;
class UAegisMap;
/**
 * 
 */
UCLASS()
class AEGIS_API AAegisGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

public:
	UPROPERTY()
	UAegisMap* AegisMap;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Map Factory")
	TSubclassOf<UAegisMapFactory> MapFactoryClass;
	
protected:
};
