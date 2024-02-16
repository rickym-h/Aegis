// Fill out your copyright notice in the Description page of Project Settings.


#include "AegisGameStateBase.h"

#include "Enemies/EnemyFactory.h"
#include "Map/AegisMapFactory.h"
#include "Structures/Towers/TowerDataFactory.h"

void AAegisGameStateBase::BeginPlay()
{
	Super::BeginPlay();
	
	UE_LOG(LogTemp, Warning, TEXT("AAegisGameStateBase::BeginPlay()"))
}

void AAegisGameStateBase::PostInitializeComponents()
{
	UE_LOG(LogTemp, Warning, TEXT("AAegisGameStateBase::PostInitializeComponents()"))
	Super::PostInitializeComponents();

	if (MapFactoryClass)
	{
		MapFactory = NewObject<UAegisMapFactory>(GetWorld(), MapFactoryClass);
		AegisMap = MapFactory->GenerateTestMap();
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
