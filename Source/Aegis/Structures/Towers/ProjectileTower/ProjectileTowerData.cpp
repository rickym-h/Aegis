// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileTowerData.h"

#include "ProjectileTower.h"
#include "Kismet/GameplayStatics.h"

UProjectileTowerData::UProjectileTowerData()
{
}

ATower* UProjectileTowerData::SpawnTowerFromData(const FVector LocationToSpawnTower) const
{
	if (!TowerBlueprintClass) { return nullptr; }

	const FTransform ActorTransform = FTransform(LocationToSpawnTower);
	AProjectileTower* NewTower = GetWorld()->SpawnActorDeferred<AProjectileTower>(TowerBlueprintClass, ActorTransform);
	//
	UE_LOG(LogTemp, Warning, TEXT("This TileCoord is at FVector: %ls"), *LocationToSpawnTower.ToString())
	NewTower->CurrentLocation = FTileCoord::FromLocation(LocationToSpawnTower);
	UE_LOG(LogTemp, Warning, TEXT("Converting back to FTileCoord: %ls"), *NewTower->CurrentLocation.ToString())
	// NewTower->TowerRange = 2;
	//
	UGameplayStatics::FinishSpawningActor(NewTower, ActorTransform);

	UE_LOG(LogTemp, Warning, TEXT("UProjectileTowerData::SpawnTowerFromData - Successfully spawned tower!"))
	return NewTower;
}
