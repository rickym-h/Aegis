// Fill out your copyright notice in the Description page of Project Settings.


#include "AegisGameStateBase.h"

#include "Map/AegisMapFactory.h"
#include "Map/MapTile.h"

void AAegisGameStateBase::BeginPlay()
{
	Super::BeginPlay();

	const UAegisMapFactory* MapFactory = NewObject<UAegisMapFactory>(GetWorld(), MapFactoryClass);
	
	// AegisMap = UAegisMapFactory::GenerateTestMap();
	AegisMap = MapFactory->GenerateTestMap();
}
