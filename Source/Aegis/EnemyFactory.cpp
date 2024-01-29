// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFactory.h"

#include "Pawns/Enemies/BaseEnemy.h"


void UEnemyFactory::SpawnTestEnemy()
{
	const FVector Location = FVector::ZeroVector;
	const FRotator Rotation = FRotator::ZeroRotator;

	ABaseEnemy* Enemy = GetWorld()->SpawnActor<ABaseEnemy>(TestEnemyClass, Location, Rotation);
	EnemiesInWorld.Add(Enemy);
}
