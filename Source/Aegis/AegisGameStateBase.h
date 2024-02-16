// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "AegisGameStateBase.generated.h"

class UTowerDataFactory;
class UEnemyFactory;
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

	virtual void PostInitializeComponents() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Map")
	UAegisMap* AegisMap;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Factories")
	UAegisMapFactory* MapFactory;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Factories")
	UEnemyFactory* EnemyFactory;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Factories")
	UTowerDataFactory* TowerDataFactory;
	
protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Factory Classes")
	TSubclassOf<UAegisMapFactory> MapFactoryClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Factory Classes")
	TSubclassOf<UEnemyFactory> EnemyFactoryClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Factory Classes")
	TSubclassOf<UTowerDataFactory> TowerDataFactoryClass;
};
