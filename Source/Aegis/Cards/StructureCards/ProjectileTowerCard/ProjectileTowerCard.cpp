// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileTowerCard.h"

#include "Aegis/Core/GameStates/AegisGameStateBase.h"
#include "Aegis/Map/AegisGameMap.h"
#include "Aegis/Structures/Towers/ProjectileTower/ProjectileTower.h"
#include "Kismet/GameplayStatics.h"

bool UProjectileTowerCard::PlayCard_Implementation(const FTileCoord& LocationCoord)
{
	const AAegisGameStateBase* GameState = Cast<AAegisGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
	
	// Get the map, and attempt to build an instance of the archer tower on the map
	AStructure* Structure = GameState->AegisMap->AddStructureToMap(this, LocationCoord);
	if (!Structure)
	{
		UE_LOG(LogTemp, Error, TEXT("UProjectileTowerCard::PlayCard_Implementation - Failed to add structure to map!"))
		return false;
	}
	UE_LOG(LogTemp, Display, TEXT("UProjectileTowerCard::PlayCard_Implementation - Successfully added structure to map!"))

	// Set structure data (e.g. tower stats)
	if (AProjectileTower* ProjectileTower = Cast<AProjectileTower>(Structure))
	{
		ProjectileTower->InitProjectileTower(ProjectileType, ProjectileDamagePackage, ProjectileMesh, AttackSpeed, RangeTiles, SlowEffect);
	} else
	{
		UE_LOG(LogTemp, Error, TEXT("UProjectileTowerCard::PlayCard_Implementation - Structure not projectile tower! Failed to InitProjectileTower()"))
	}
	

	UGameplayStatics::FinishSpawningActor(Structure, Structure->ActorTransform);
	UE_LOG(LogTemp, Display, TEXT("UProjectileTowerCard::PlayCard_Implementation - Finished spawning structure!"))
	
	// If successful, return true
	return true;
}

TSet<FTileCoord> UProjectileTowerCard::GetRangeTileOffsets_Implementation()
{
	return TSet(FTileCoord::GetTilesInRadius(FTileCoord(), RangeTiles));
}

TSet<FTileCoord> UProjectileTowerCard::RotateTileRangeOffsets_Implementation(const bool bClockwise)
{
	// No rotation because range is same in every direction
	return GetRangeTileOffsets_Implementation();
}
