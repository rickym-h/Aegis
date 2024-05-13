// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFactory.h"

#include "Enemy.h"

void UEnemyFactory::SpawnTestEnemy()
{
	const FVector Location = FVector::ZeroVector;
	const FRotator Rotation = FRotator::ZeroRotator;

	if (TestEnemyClass)
	{
		AEnemy* Enemy = GetWorld()->SpawnActor<AEnemy>(TestEnemyClass, Location, Rotation);
		EnemiesInWorld.Add(Enemy);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UEnemyFactory::SpawnTestEnemy() - No TestEnemyClass"))
	}
}
