// Fill out your copyright notice in the Description page of Project Settings.


#include "StructureDataFactory.h"

#include "Towers/ProjectileTower/ProjectileTowerData.h"

UStructureData* UStructureDataFactory::GenerateTestTowerData() const
{
	if (!TestTowerDataClass) { return nullptr; }

	UProjectileTowerData* TowerData = NewObject<UProjectileTowerData>(WorldReference, TestTowerDataClass);

	TowerData->TowerRange = 3;

	return TowerData;
}
