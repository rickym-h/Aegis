// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFactory.h"
#include "Enemy.h"

void UEnemyFactory::RemoveEnemyFromWorld(AActor* DestroyedActor)
{
	AEnemy* Enemy = Cast<AEnemy>(DestroyedActor);
	if (!Enemy)
	{
		UE_LOG(LogTemp, Error, TEXT("UEnemyFactory::RemoveEnemyFromWorld() - Attempted to remove an AActor that is not an enemy."))
		return;
	}
	
	EnemiesInWorld.Remove(Enemy);

	if (EnemiesInWorld.Num() == 0 && EnemiesToSpawnInWave.Num()==0)
	{
		//UE_LOG(LogTemp, Warning, TEXT("UEnemyFactory::RemoveEnemyFromWorld() - Wave complete!"))
		OnWaveEndDelegate.Broadcast();
	}
}

TArray<FEnemySpawnData> UEnemyFactory::GenerateWaveEnemies(const int32 WorldLayer) const
{
	TArray<FEnemySpawnData> WaveEnemies = TArray<FEnemySpawnData>();
	if (EnemiesToSpawnInWave.Num() != 0)
	{
		UE_LOG(LogTemp, Fatal, TEXT("UEnemyFactory::BeginWave - Cannot start wave because there are still enemies to spawn in current wave."))
		return TArray<FEnemySpawnData>();
	}
	// const int32 EnemiesToSpawnCount = Night * Wave;
	const int32 EnemiesToSpawnCount = 5;

	for (int i = 0; i < EnemiesToSpawnCount; i++)
	{
		FEnemySpawnData SpawnData = FEnemySpawnData(
			TestEnemyClass,
			0.9f,
			0.1f);

		WaveEnemies.Add(SpawnData);
	}

	return WaveEnemies;
}

void UEnemyFactory::SpawnEnemyFromWave()
{
	// Spawn the enemy at the top of the wave list
	const FEnemySpawnData CurrentEnemySpawnData = EnemiesToSpawnInWave.Pop();
	SpawnTestEnemy(CurrentEnemySpawnData.EnemyClass);
	
	// Start the timer for the next enemy to spawn
	if(EnemiesToSpawnInWave.Num() != 0)
	{
		const FEnemySpawnData NextEnemySpawnData = EnemiesToSpawnInWave.Top();

		GetWorld()->GetTimerManager().SetTimer(
			SpawnEnemyWithDelayTimerHandle,
			this,
			&UEnemyFactory::SpawnEnemyFromWave,
			NextEnemySpawnData.PreSpawnDelay + CurrentEnemySpawnData.PostSpawnDelay,
			false); 
	}
}

void UEnemyFactory::BeginWave(const int32 WorldLayer)
{
	// Ensure wave is able to begin (no enemies in world, no enemies still to be spawned from previous wave
	if (EnemiesInWorld.Num() != 0 || EnemiesToSpawnInWave.Num() != 0)
	{
		UE_LOG(LogTemp, Error, TEXT("UEnemyFactory::BeginWave - Cannot start wave because there are still enemies in world, or there are still enemies to spawn in current wave."))
		return;
	}

	WaveCounter++;

	UE_LOG(LogTemp, Warning, TEXT("UEnemyFactory::BeginWave - Starting wave on level/layer %i, Night %i, Wave %i!"), WorldLayer, NightCounter, WaveCounter)
	
	
	// Decide which enemies to be spawned
	EnemiesToSpawnInWave = GenerateWaveEnemies(WorldLayer);

	// Start Spawn of first enemy in wave
	GetWorld()->GetTimerManager().SetTimer(
		SpawnEnemyWithDelayTimerHandle,
		this,
		&UEnemyFactory::SpawnEnemyFromWave,
		EnemiesToSpawnInWave.Top().PreSpawnDelay,
		false);

}

AEnemy* UEnemyFactory::SpawnTestEnemy(TSubclassOf<AEnemy> EnemyClass)
{
	const FVector Location = FVector::ZeroVector;
	const FRotator Rotation = FRotator::ZeroRotator;

	if (!EnemyClass)
	{
		if (!TestEnemyClass)
		{
			UE_LOG(LogTemp, Error, TEXT("UEnemyFactory::SpawnTestEnemy() - No TestEnemyClass"))
			return nullptr;
		}
		EnemyClass = TestEnemyClass;
	}

	AEnemy* Enemy = GetWorld()->SpawnActor<AEnemy>(EnemyClass, Location, Rotation);
	EnemiesInWorld.Add(Enemy);

	Enemy->OnDestroyed.AddUniqueDynamic(this, &UEnemyFactory::RemoveEnemyFromWorld);

	return Enemy;
}

int32 UEnemyFactory::GetWaveCounter() const
{
	return WaveCounter;
}

int32 UEnemyFactory::GetNightCounter() const
{
	return NightCounter;
}
