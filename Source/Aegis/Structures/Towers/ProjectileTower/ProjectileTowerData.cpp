// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileTowerData.h"

#include "ProjectileTower.h"
#include "Kismet/GameplayStatics.h"

AStructure* UProjectileTowerData::SpawnStructureFromData(const FTileCoord CoordToSpawnStructure, const FVector BuildingLocation) const
{
	if (!StructureBlueprintClass) { return nullptr; }

	const FTransform ActorTransform = FTransform(BuildingLocation);
	AProjectileTower* NewStructure = GetWorld()->SpawnActorDeferred<AProjectileTower>(StructureBlueprintClass, ActorTransform);

	NewStructure->CurrentLocation = CoordToSpawnStructure;
	NewStructure->ProjectileMesh = ProjectileMesh;
	NewStructure->ProjectileSpeed = ProjectileSpeed;
	NewStructure->AttackSpeed = AttackSpeed;

	UGameplayStatics::FinishSpawningActor(NewStructure, ActorTransform);
	NewStructure->SetTowerRange(TowerRange);

	return NewStructure;
}
