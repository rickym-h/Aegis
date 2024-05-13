// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyFactory.generated.h"

class AEnemy;
class AAegisGameStateBase;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class AEGIS_API UEnemyFactory : public UObject
{
	GENERATED_BODY()

protected:

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Enemies")
	TSubclassOf<AEnemy> TestEnemyClass;

	TArray<AEnemy*> EnemiesInWorld;

	UFUNCTION(BlueprintCallable)
	void SpawnTestEnemy();
};
