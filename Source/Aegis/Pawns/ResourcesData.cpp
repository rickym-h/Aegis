// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourcesData.h"

void UResourcesData::InitResources(const FResources& InResources)
{
	Resources = InResources;
}

void UResourcesData::AddResources(const int32 InWood, const int32 InStone, const int32 InGold, const int32 InRunes, const int32 InManaCrystals)
{
	Resources.Wood+=InWood;
	Resources.Stone+=InStone;
	Resources.Gold+=InGold;
	Resources.Runes+=InRunes;
	Resources.ManaCrystals+=InManaCrystals;
	OnResourcesUpdatedDelegate.Broadcast();
}

void UResourcesData::AddResources(const UResourcesData* ResourcesToAdd)
{
	AddResources(
		ResourcesToAdd->GetWood(),
		ResourcesToAdd->GetStone(),
		ResourcesToAdd->GetGold(),
		ResourcesToAdd->GetRunes(),
		ResourcesToAdd->GetManaCrystals());
}

void UResourcesData::AddResources(const FResources& ResourcesToAdd)
{
	AddResources(
		ResourcesToAdd.Wood,
		ResourcesToAdd.Stone,
		ResourcesToAdd.Gold,
		ResourcesToAdd.Runes,
		ResourcesToAdd.ManaCrystals);
}

bool UResourcesData::IsResourcesEnough(const FResources& ResourcesCost) const
{
	return (
		Resources.Wood >= ResourcesCost.Wood &&
		Resources.Stone >= ResourcesCost.Stone &&
		Resources.Gold >= ResourcesCost.Gold &&
		Resources.Runes >= ResourcesCost.Runes &&
		Resources.ManaCrystals >= ResourcesCost.ManaCrystals
		);
}

void UResourcesData::SpendResources(const FResources& Cost)
{
	Resources.Wood -= Cost.Wood;
	Resources.Stone -= Cost.Stone;
	Resources.Gold -= Cost.Gold;
	Resources.Runes -= Cost.Runes;
	Resources.ManaCrystals -= Cost.ManaCrystals;
	OnResourcesUpdatedDelegate.Broadcast();
}

FResources UResourcesData::GetResources() const
{
	return Resources;
}

int32 UResourcesData::GetWood() const
{
	return Resources.Wood;
}

int32 UResourcesData::GetStone() const
{
	return Resources.Stone;
}

int32 UResourcesData::GetGold() const
{
	return Resources.Gold;
}

int32 UResourcesData::GetRunes() const
{
	return Resources.Runes;
}

int32 UResourcesData::GetManaCrystals() const
{
	return Resources.ManaCrystals;
}
