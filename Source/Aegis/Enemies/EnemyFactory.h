// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyFactory.generated.h"

class AEnemy;

USTRUCT(BlueprintType)
struct FEnemySpawnData
{
	GENERATED_BODY()

	FEnemySpawnData()
	{
		EnemyClass = nullptr;
		PreSpawnDelay = 0.f;
		PostSpawnDelay = 0.f;
	}

	FEnemySpawnData(const TSubclassOf<AEnemy> InEnemyClass, const float InPreSpawnDelay, const float InPostSpawnDelay)
	{
		EnemyClass = InEnemyClass;
		PreSpawnDelay = InPreSpawnDelay;
		PostSpawnDelay = InPostSpawnDelay;
	}
	
	UPROPERTY(EditAnywhere, Category="Enemy Info")
	TSubclassOf<AEnemy> EnemyClass;
	
	UPROPERTY(EditAnywhere, Category="Spawn Info")
	float PreSpawnDelay = 0.f;
	UPROPERTY(EditAnywhere, Category="Spawn Info")
	float PostSpawnDelay = 0.f;
};

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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="WaveEnemies")
	TArray<FEnemySpawnData> EnemiesToSpawnInWave;
	
	TArray<FEnemySpawnData> GenerateWaveEnemies(const int32 Night, const int32 Wave) const;

	FTimerHandle SpawnEnemyWithDelayTimerHandle;
	
	void SpawnEnemyFromWave();

public:
	UFUNCTION(BlueprintCallable)
	void BeginWave(const int32 Night, const int32 Wave);

	UFUNCTION(BlueprintCallable)
	AEnemy* SpawnTestEnemy(TSubclassOf<AEnemy> EnemyClass);
};
