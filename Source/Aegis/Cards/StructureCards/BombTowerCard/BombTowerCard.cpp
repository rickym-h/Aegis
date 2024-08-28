// Fill out your copyright notice in the Description page of Project Settings.


#include "BombTowerCard.h"

#include "Aegis/Core/GameStates/AegisGameStateBase.h"
#include "Aegis/Map/AegisGameMap.h"
#include "Aegis/Structures/Towers/BombTower/BombTower.h"
#include "Kismet/GameplayStatics.h"

bool UBombTowerCard::PlayCard_Implementation(const FTileCoord& LocationCoord)
{
	const AAegisGameStateBase* GameState = Cast<AAegisGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
	
	// Get the map, and attempt to build an instance of the archer tower on the map
	AStructure* Structure = GameState->AegisMap->AddStructureToMap(this, LocationCoord);
	if (!Structure)
	{
		UE_LOG(LogTemp, Error, TEXT("UBombTowerCard::PlayCard_Implementation - Failed to add structure to map!"))
		return false;
	}
	UE_LOG(LogTemp, Display, TEXT("UBombTowerCard::PlayCard_Implementation - Successfully added structure to map!"))

	// Set structure data (e.g. tower stats)
	if (ABombTower* BombTower = Cast<ABombTower>(Structure))
	{
		BombTower->InitBombTowerData(RangeInMetres, ShotsPerSecond, DamagePerShot, ExplosionRadius);
	} else
	{
		UE_LOG(LogTemp, Error, TEXT("UBombTowerCard::PlayCard_Implementation - Structure not bomb tower! Failed to InitBombTowerData()"))
	}
	

	UGameplayStatics::FinishSpawningActor(Structure, Structure->ActorTransform);
	UE_LOG(LogTemp, Display, TEXT("UBombTowerCard::PlayCard_Implementation - Finished spawning structure!"))
	
	// If successful, return true
	return true;
}

float UBombTowerCard::GetRangeInMetres_Implementation()
{
	return RangeInMetres;
}
