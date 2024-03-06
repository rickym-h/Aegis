// Fill out your copyright notice in the Description page of Project Settings.


#include "StructureData.h"

TSubclassOf<AStructure> UStructureData::GetStructureBlueprintClass() const
{
	return StructureBlueprintClass;
}
