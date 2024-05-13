// Fill out your copyright notice in the Description page of Project Settings.


#include "StructureDataFactory.h"

#include "Buildings/ResourceCollector/ResourceCollectorData.h"
#include "Towers/ProjectileTower/ProjectileTowerData.h"

TArray<UStructureData*> UStructureDataFactory::GenerateStarterTowers(UWorld* WorldRef) const
{
	TArray<UStructureData*> StarterTowers;

	StarterTowers.Add(GenerateTestTowerData(WorldRef));
	StarterTowers.Add(GenerateLumberYardData(WorldRef));

	return StarterTowers;
}

UStructureData* UStructureDataFactory::GenerateTestTowerData(UWorld* WorldRef) const
{
	if (!TestTowerDataClass) { return nullptr; }

	UProjectileTowerData* StructureData = NewObject<UProjectileTowerData>(WorldRef, TestTowerDataClass);

	return StructureData;
}

UStructureData* UStructureDataFactory::GenerateLumberYardData(UWorld* WorldRef) const
{
	if (!LumberYardDataClass) { return nullptr; }

	UResourceCollectorData* StructureData = NewObject<UResourceCollectorData>(WorldRef, LumberYardDataClass);

	return StructureData;
}
