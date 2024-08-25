// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldMapGameStateBase.h"

#include "Aegis/Map/AegisMapFactory.h"

void AWorldMapGameStateBase::BeginPlay()
{
	Super::BeginPlay();
	
	GenerateWorldMapFromMapFactory();
}

void AWorldMapGameStateBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	if (MapFactoryClass)
	{
		MapFactory = NewObject<UAegisMapFactory>(GetWorld(), MapFactoryClass);
	}
}

void AWorldMapGameStateBase::GenerateWorldMapFromMapFactory()
{
	WorldMap = MapFactory->GenerateWorldMap();
}
