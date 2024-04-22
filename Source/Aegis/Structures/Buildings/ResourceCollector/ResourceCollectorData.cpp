// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourceCollectorData.h"

#include "ResourceCollector.h"
#include "Aegis/Structures/Structure.h"
#include "Kismet/GameplayStatics.h"

AStructure* UResourceCollectorData::SpawnStructureFromData(const FTileCoord CoordToSpawnStructure, const FVector BuildingLocation) const
{
	if (!StructureBlueprintClass) { return nullptr; }

	const FTransform ActorTransform = FTransform(BuildingLocation);
	AResourceCollector* NewStructure = GetWorld()->SpawnActorDeferred<AResourceCollector>(StructureBlueprintClass, ActorTransform);

	NewStructure->CurrentLocation = CoordToSpawnStructure;

	UGameplayStatics::FinishSpawningActor(NewStructure, ActorTransform);

	return NewStructure;
}
