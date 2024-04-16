// Fill out your copyright notice in the Description page of Project Settings.


#include "StructureDataFactory.h"

#include "Buildings/ResourceCollector/ResourceCollectorData.h"
#include "Towers/ProjectileTower/ProjectileTowerData.h"

UStructureData* UStructureDataFactory::GenerateTestTowerData() const
{
	if (!TestTowerDataClass) { return nullptr; }

	UProjectileTowerData* StructureData = NewObject<UProjectileTowerData>(WorldReference, TestTowerDataClass);

	StructureData->TowerRange = 3;

	return StructureData;
}

UStructureData* UStructureDataFactory::GenerateLumberYardData() const
{
	if (!LumberYardDataClass) { return nullptr; }

	UResourceCollectorData* StructureData = NewObject<UResourceCollectorData>(WorldReference, LumberYardDataClass);

	return StructureData;
}
