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
	TArray<AEnemy*> EnemiesInWorld;

	UFUNCTION(BlueprintCallable)
	void RemoveEnemyFromWorld(AActor* DestroyedActor);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Enemies")
	TSubclassOf<AEnemy> TestEnemyClass;

public:

	UFUNCTION(BlueprintCallable)
	void BeginWave(const int32 Night, const int32 Wave);

	UFUNCTION(BlueprintCallable)
	AEnemy* SpawnTestEnemy(TSubclassOf<AEnemy> EnemyClass);
};
