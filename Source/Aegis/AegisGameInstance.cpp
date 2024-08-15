// Fill out your copyright notice in the Description page of Project Settings.


#include "AegisGameInstance.h"

#include "Structures/StructureDataFactory.h"

void UAegisGameInstance::OnStart()
{
	Super::OnStart();

	if (StructureDataFactoryClass)
	{
		StructureDataFactory = NewObject<UStructureDataFactory>(this, StructureDataFactoryClass);
	}
}
