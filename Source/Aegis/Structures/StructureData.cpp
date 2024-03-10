// Fill out your copyright notice in the Description page of Project Settings.


#include "StructureData.h"

UStaticMesh* UStructureData::GetMeshRepresentation() const
{
	return MeshRepresentation;
}

TSubclassOf<AStructure> UStructureData::GetStructureBlueprintClass() const
{
	return StructureBlueprintClass;
}
