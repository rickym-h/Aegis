// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourcesData.h"

void UResourcesData::AddResources(const int32 InWood, const int32 InStone, const int32 InGold, const int32 InRunes, const int32 InManaCrystals)
{
	Wood+=InWood;
	Stone+=InStone;
	Gold+=InGold;
	Runes+=InRunes;
	ManaCrystals+=InManaCrystals;
	OnResourcesUpdatedDelegate.Broadcast();
}

int32 UResourcesData::GetWood() const
{
	return Wood;
}

int32 UResourcesData::GetStone() const
{
	return Stone;
}

int32 UResourcesData::GetGold() const
{
	return Gold;
}

int32 UResourcesData::GetRunes() const
{
	return Runes;
}

int32 UResourcesData::GetManaCrystals() const
{
	return ManaCrystals;
}
