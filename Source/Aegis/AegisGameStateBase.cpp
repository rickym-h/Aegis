// Fill out your copyright notice in the Description page of Project Settings.


#include "AegisGameStateBase.h"

#include "Enemies/EnemyFactory.h"
#include "Map/AegisMapFactory.h"
#include "Structures/Towers/TowerDataFactory.h"

void AAegisGameStateBase::BeginPlay()
{
	Super::BeginPlay();

	if (MapFactoryClass)
	{
		MapFactory = NewObject<UAegisMapFactory>(GetWorld(), MapFactoryClass);
		//GenerateTestMapFromMapFactory();
	}

	if (EnemyFactoryClass)
	{
		EnemyFactory = NewObject<UEnemyFactory>(GetWorld(), EnemyFactoryClass);
		EnemyFactory->SetOwningGameState(this);
	}

	if (TowerDataFactoryClass)
	{
		TowerDataFactory = NewObject<UTowerDataFactory>(GetWorld(), TowerDataFactoryClass);
		TowerDataFactory->WorldReference = GetWorld();
	}
}

void AAegisGameStateBase::GenerateTestMapFromMapFactory()
{
	AegisMap = MapFactory->GenerateTestMap();
}
