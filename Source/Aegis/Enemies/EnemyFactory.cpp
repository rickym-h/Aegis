// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFactory.h"

#include "Enemy.h"


void UEnemyFactory::RemoveEnemyFromWorld(AActor* DestroyedActor)
{
	if (AEnemy* Enemy = Cast<AEnemy>(DestroyedActor))
	{		
		EnemiesInWorld.Remove(Enemy);
	}
}

void UEnemyFactory::BeginWave(const int32 Night, const int32 Wave)
{
	// TODO make sure no enemies are still alive
	if (EnemiesInWorld.Num() != 0)
	{
		UE_LOG(LogTemp, Error, TEXT("UEnemyFactory::BeginWave - Cannot start wave because there are still enemies in world."))
		return;
	}

	// TODO logic for deciding enemies to spawn
	int32 EnemiesToSpawn = Night * Wave;

	

	
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
