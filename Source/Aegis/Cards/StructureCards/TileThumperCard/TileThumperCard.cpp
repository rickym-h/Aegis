// Fill out your copyright notice in the Description page of Project Settings.


#include "TileThumperCard.h"

#include "Aegis/Core/GameStates/AegisGameStateBase.h"
#include "Aegis/Map/AegisGameMap.h"
#include "Aegis/Structures/Towers/TileThumper/TileThumper.h"
#include "Kismet/GameplayStatics.h"

bool UTileThumperCard::PlayCard_Implementation(const FTileCoord& LocationCoord)
{
	const AAegisGameStateBase* GameState = Cast<AAegisGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
	
	// Get the map, and attempt to build an instance of the archer tower on the map
	AStructure* Structure = GameState->AegisMap->AddStructureToMap(this, LocationCoord);
	if (!Structure)
	{
		UE_LOG(LogTemp, Error, TEXT("UTileThumperCard::PlayCard_Implementation - Failed to add structure to map!"))
		return false;
	}
	UE_LOG(LogTemp, Display, TEXT("UTileThumperCard::PlayCard_Implementation - Successfully added structure to map!"))

	// Set structure data (e.g. tower stats)
	if (ATileThumper* TileThumper = Cast<ATileThumper>(Structure))
	{
		TileThumper->InitTileThumper(AttackSpeed, TileRangeOffsets);
	} else
	{
		UE_LOG(LogTemp, Error, TEXT("UTileThumperCard::PlayCard_Implementation - Structure not TileThumper tower! Failed to InitProjectileTower()"))
	}
	
	UGameplayStatics::FinishSpawningActor(Structure, Structure->ActorTransform);
	UE_LOG(LogTemp, Display, TEXT("UTileThumperCard::PlayCard_Implementation - Finished spawning structure!"))
	
	// If successful, return true
	return true;
}

TSet<FTileCoord> UTileThumperCard::GetRangeTileOffsets_Implementation()
{
	return TileRangeOffsets;
}
