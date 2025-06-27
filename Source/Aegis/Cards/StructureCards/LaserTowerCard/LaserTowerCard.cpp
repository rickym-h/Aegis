// Fill out your copyright notice in the Description page of Project Settings.


#include "LaserTowerCard.h"

#include "Aegis/Core/GameStates/AegisGameStateBase.h"
#include "Aegis/Map/AegisGameMap.h"
#include "Aegis/Structures/Towers/LaserTower/LaserTower.h"
#include "Kismet/GameplayStatics.h"


bool ULaserTowerCard::PlayCard_Implementation(const FTileCoord& LocationCoord)
{
	const AAegisGameStateBase* GameState = Cast<AAegisGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
	
	// Get the map, and attempt to build an instance of the laser tower on the map
	AStructure* Structure = GameState->AegisMap->AddStructureToMap(this, LocationCoord);
	if (!Structure)
	{
		UE_LOG(LogTemp, Error, TEXT("ULaserTowerCard::PlayCard_Implementation - Failed to add structure to map!"))
		return false;
	}
	UE_LOG(LogTemp, Display, TEXT("ULaserTowerCard::PlayCard_Implementation - Successfully added structure to map!"))

	// Set structure data (e.g. tower stats)
	if (ALaserTower* LaserTower = Cast<ALaserTower>(Structure))
	{
		LaserTower->InitLaserTower(RangeTiles, DamagePerSecond, LaseFireRate);
	} else
	{
		UE_LOG(LogTemp, Error, TEXT("ULaserTowerCard::PlayCard_Implementation - Structure not laser tower! Failed to InitLaserTower()"))
	}
	

	UGameplayStatics::FinishSpawningActor(Structure, Structure->ActorTransform);
	UE_LOG(LogTemp, Display, TEXT("ULaserTowerCard::PlayCard_Implementation - Finished spawning structure!"))
	
	// If successful, return true
	return true;
}

TSet<FTileCoord> ULaserTowerCard::GetRangeTileOffsets_Implementation()
{
	return TSet(FTileCoord::GetTilesInRadius(FTileCoord(), RangeTiles));
}

TSet<FTileCoord> ULaserTowerCard::RotateTileRangeOffsets_Implementation(const bool bClockwise)
{
	// No rotation because range is same in every direction
	return GetRangeTileOffsets_Implementation();
}
