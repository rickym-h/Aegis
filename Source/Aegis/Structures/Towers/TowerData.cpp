// Fill out your copyright notice in the Description page of Project Settings.


#include "TowerData.h"

TSubclassOf<ATower> UTowerData::GetTowerBlueprintClass() const
{
	return TowerBlueprintClass;
}
