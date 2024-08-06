// Fill out your copyright notice in the Description page of Project Settings.


#include "StructureDataFactory.h"

#include "Towers/ProjectileTower/ProjectileTowerData.h"

TArray<UStructureData*> UStructureDataFactory::GenerateStarterTowers(UWorld* WorldRef) const
{
	TArray<UStructureData*> StarterTowers;

	for (const TSubclassOf<UStructureData> StarterTowerDataClass : StarterTowersData)
	{
		if (!StarterTowerDataClass)
		{
			UE_LOG(LogTemp, Error, TEXT("UStructureDataFactory::GenerateStarterTowers() - StarterTowerData is null"))
		}

		UStructureData* StructureData = NewObject<UStructureData>(WorldRef, StarterTowerDataClass);
		StructureData->bRemoveInstanceOnPlacement = true;
		StarterTowers.Add(StructureData);
	}

	return StarterTowers;
}