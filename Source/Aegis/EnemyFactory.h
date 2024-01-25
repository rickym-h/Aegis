// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyFactory.generated.h"

class ABaseEnemy;
class AAegisGameStateBase;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class AEGIS_API UEnemyFactory : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	AAegisGameStateBase* OwningGameState;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Enemies")
	TSubclassOf<ABaseEnemy> TestEnemyClass;

	TArray<ABaseEnemy*> EnemiesInWorld;

	UFUNCTION(BlueprintCallable)
	void SpawnTestEnemy();
};
