// Fill out your copyright notice in the Description page of Project Settings.


#include "SlowingProjectileTowerCard.h"

#include "Aegis/Core/GameStates/AegisGameStateBase.h"
#include "Aegis/Map/AegisGameMap.h"
#include "Aegis/Structures/Towers/ProjectileTower/SlowingProjectileTower/SlowingProjectileTower.h"
#include "Kismet/GameplayStatics.h"

bool USlowingProjectileTowerCard::PlayCard_Implementation(const FTileCoord& LocationCoord)
{
	const AAegisGameStateBase* GameState = Cast<AAegisGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
	
	// Get the map, and attempt to build an instance of the tower on the map
	AStructure* Structure = GameState->AegisMap->AddStructureToMap(this, LocationCoord);
	if (!Structure)
	{
		UE_LOG(LogTemp, Error, TEXT("USlowingProjectileTowerCard::PlayCard_Implementation - Failed to add structure to map!"))
		return false;
	}
	UE_LOG(LogTemp, Display, TEXT("USlowingProjectileTowerCard::PlayCard_Implementation - Successfully added structure to map!"))

	// Set structure data (e.g. tower stats)
	if (ASlowingProjectileTower* SlowingProjectileTower = Cast<ASlowingProjectileTower>(Structure))
	{
		SlowingProjectileTower->InitSlowingProjectileTower(ProjectileDamagePackage, AttackSpeed, RangeTiles, ProjectileMesh, EffectToApply);
	} else
	{
		UE_LOG(LogTemp, Error, TEXT("USlowingProjectileTowerCard::PlayCard_Implementation - Structure not projectile tower! Failed to InitSlowingProjectileTower()"))
	}
	

	UGameplayStatics::FinishSpawningActor(Structure, Structure->ActorTransform);
	UE_LOG(LogTemp, Display, TEXT("USlowingProjectileTowerCard::PlayCard_Implementation - Finished spawning structure!"))
	
	// If successful, return true
	return true;
}