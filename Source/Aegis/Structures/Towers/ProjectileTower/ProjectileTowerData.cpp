// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileTowerData.h"

#include "ProjectileTower.h"
#include "Kismet/GameplayStatics.h"

UProjectileTowerData::UProjectileTowerData()
{
}

AStructure* UProjectileTowerData::SpawnStructureFromData(const FTileCoord CoordToSpawnStructure) const
{
	if (!StructureBlueprintClass) { return nullptr; }

	const FTransform ActorTransform = FTransform(CoordToSpawnStructure.ToWorldLocation());
	AProjectileTower* NewTower = GetWorld()->SpawnActorDeferred<AProjectileTower>(StructureBlueprintClass, ActorTransform);

	NewTower->CurrentLocation = CoordToSpawnStructure;
	NewTower->SetTowerRange(2);

	UGameplayStatics::FinishSpawningActor(NewTower, ActorTransform);

	UE_LOG(LogTemp, Warning, TEXT("UProjectileTowerData::SpawnStructureFromData - Successfully spawned tower!"))
	return NewTower;
}
