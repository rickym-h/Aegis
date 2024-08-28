// Fill out your copyright notice in the Description page of Project Settings.


#include "ArcherCard.h"

#include "Aegis/Core/GameStates/AegisGameStateBase.h"
#include "Aegis/Map/AegisGameMap.h"
#include "Aegis/Map/AegisMap.h"
#include "Aegis/Structures/Structure.h"
#include "Aegis/Structures/Towers/ArcherTower/ArcherTower.h"
#include "Kismet/GameplayStatics.h"

bool UArcherCard::PlayCard_Implementation(const FTileCoord& LocationCoord)
{
	const AAegisGameStateBase* GameState = Cast<AAegisGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
	
	// Get the map, and attempt to build an instance of the archer tower on the map
	AStructure* Structure = GameState->AegisMap->AddStructureToMap(this, LocationCoord);
	if (!Structure)
	{
		UE_LOG(LogTemp, Error, TEXT("UArcherCard::PlayCard_Implementation - Failed to add structure to map!"))
		return false;
	}
	UE_LOG(LogTemp, Display, TEXT("UArcherCard::PlayCard_Implementation - Successfully added structure to map!"))

	// Set structure data (e.g. tower stats)
	if (AArcherTower* ArcherTower = Cast<AArcherTower>(Structure))
	{
		ArcherTower->InitArcherTowerData(RangeInMetres, ShotsPerSecond, DamagePerShot);
	} else
	{
		UE_LOG(LogTemp, Error, TEXT("UArcherCard::PlayCard_Implementation - Structure not archer tower! Failed to InitArcherTowerData()"))
	}
	

	UGameplayStatics::FinishSpawningActor(Structure, Structure->ActorTransform);
	UE_LOG(LogTemp, Display, TEXT("UArcherCard::PlayCard_Implementation - Finished spawning structure!"))
	
	// If successful, return true
	return true;
}

float UArcherCard::GetRangeInMetres_Implementation()
{
	return RangeInMetres;
}
