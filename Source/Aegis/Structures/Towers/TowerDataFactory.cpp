// Fill out your copyright notice in the Description page of Project Settings.


#include "TowerDataFactory.h"

#include "Tower.h"
#include "TowerData.h"
#include "ProjectileTower/ProjectileTowerData.h"

UTowerData* UTowerDataFactory::GenerateTestTowerData() const
{
	if (!TestTowerDataClass) { return nullptr; }

	UProjectileTowerData* TowerData = NewObject<UProjectileTowerData>(WorldReference, TestTowerDataClass);

	TowerData->TowerRange = 3;

	return TowerData;
}
