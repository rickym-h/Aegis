// Fill out your copyright notice in the Description page of Project Settings.


#include "StructureDataFactory.h"

#include "StructureData.h"


TArray<UStructureData*> UStructureDataFactory::GenerateStarterTowers(UObject* Outer) const
{
	TArray<UStructureData*> StarterTowers;

	for (const TSubclassOf<UStructureData> StarterTowerDataClass : StarterTowersData)
	{
		if (!StarterTowerDataClass)
		{
			UE_LOG(LogTemp, Error, TEXT("UStructureDataFactory::GenerateStarterTowers() - StarterTowerData is null"))
		}

		UStructureData* StructureData = NewObject<UStructureData>(Outer, StarterTowerDataClass);
		StructureData->bRemoveInstanceOnPlacement = true;
		StarterTowers.Add(StructureData);
	}

	return StarterTowers;
}
