// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "EnemyFactory.generated.h"

class AEnemy;
class AAegisGameStateBase;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWaveEndSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveCounterChangedSignature, int32, NewWaveCounter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNightCounterChangedSignature, int32, NewNightCounter);

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

USTRUCT(BlueprintType, meta=(ShowOnlyInnerProperties))
struct FEnemyType
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<AEnemy> EnemyClass;

	UPROPERTY(EditAnywhere)
	uint32 EnemyValue = 1;

	UPROPERTY(EditAnywhere)
	uint32 SelectionWeight = 1;
};

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class AEGIS_API UEnemyFactory : public UObject
{
	GENERATED_BODY()

	UEnemyFactory();

protected:
	UPROPERTY()
	TArray<AEnemy*> EnemiesInWorld;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wave")
	int32 NightCounter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wave")
	int32 WaveCounter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wave")
	int32 MaxNightCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wave")
	int32 MaxWaveCount;

	UFUNCTION(BlueprintCallable)
	void RemoveEnemyFromWorld(AActor* DestroyedActor);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Enemies")
	TSubclassOf<AEnemy> TestEnemyClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="WaveEnemies")
	TArray<FEnemySpawnData> EnemiesToSpawnInWave;
	
	TArray<FEnemySpawnData> GenerateWaveEnemies(const int32 WorldLayer) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(RequiredAssetDataTags = "RowStructure=/Script/Aegis.EnemyData"))
	UDataTable* EnemyDataTable;

	FTimerHandle SpawnEnemyWithDelayTimerHandle;
	
	void SpawnEnemyFromWave();

public:

	UPROPERTY(BlueprintAssignable)
	FOnWaveCounterChangedSignature OnWaveCounterChangedDelegate;
	UPROPERTY(BlueprintAssignable)
	FOnNightCounterChangedSignature OnNightCounterChangedDelegate;
	
	UFUNCTION(BlueprintCallable)
	void BeginWave(const int32 WorldLayer);

	UFUNCTION(BlueprintCallable)
	AEnemy* SpawnTestEnemy(TSubclassOf<AEnemy> EnemyClass);

	UPROPERTY(BlueprintAssignable)
	FOnWaveEndSignature OnWaveEndDelegate;

	UFUNCTION(BlueprintCallable)
	int32 GetWaveCounter() const;
	UFUNCTION(BlueprintCallable)
	int32 GetNightCounter() const;
};
