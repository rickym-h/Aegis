// Fill out your copyright notice in the Description page of Project Settings.


#include "ArcherTowerData.h"

#include "ArcherTower.h"
#include "Aegis/Structures/Structure.h"
#include "Kismet/GameplayStatics.h"

AStructure* UArcherTowerData::SpawnStructureFromData(const FTileCoord CoordToSpawnStructure, const FVector BuildingLocation,
                                                     UStructureData* StructureData) const
{
	if (!StructureBlueprintClass)
	{
		UE_LOG(LogTemp, Error, TEXT("UArcherTowerData::SpawnStructureFromData - Strucutre Blueprint class not found!!"))
	}

	const FTransform ActorTransform = FTransform(BuildingLocation);

	// Spawn the actor in world
	AArcherTower* ArcherTower = GetWorld()->SpawnActorDeferred<AArcherTower>(StructureBlueprintClass, ActorTransform);
	ArcherTower->SetStructureData(StructureData, CoordToSpawnStructure);
	UGameplayStatics::FinishSpawningActor(ArcherTower, ActorTransform);

	ArcherTower->InitArcherTowerData(RangeInMetres, ShotsPerSecond, Damage);

	return ArcherTower;
}
