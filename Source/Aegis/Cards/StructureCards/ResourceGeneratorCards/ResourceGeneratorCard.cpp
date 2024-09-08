// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourceGeneratorCard.h"

#include "Aegis/Core/GameStates/AegisGameStateBase.h"
#include "Aegis/Map/AegisGameMap.h"
#include "Aegis/Structures/ResourceCollectors/ResourceCollector.h"
#include "Kismet/GameplayStatics.h"

bool UResourceGeneratorCard::PlayCard_Implementation(const FTileCoord& LocationCoord)
{
	const AAegisGameStateBase* GameState = Cast<AAegisGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
	
	// Get the map, and attempt to build an instance of the archer tower on the map
	AStructure* Structure = GameState->AegisMap->AddStructureToMap(this, LocationCoord);
	if (!Structure)
	{
		UE_LOG(LogTemp, Error, TEXT("UResourceGeneratorCard::PlayCard_Implementation - Failed to add structure to map!"))
		return false;
	}
	UE_LOG(LogTemp, Display, TEXT("UResourceGeneratorCard::PlayCard_Implementation - Successfully added structure to map!"))

	// Set structure data (e.g. tower stats)
	if (AResourceCollector* ResourceCollector = Cast<AResourceCollector>(Structure))
	{
		ResourceCollector->InitResourceCollector(WaveEndGeneratedResources, ResourcesToGeneratePerAdjacentResource);
	} else
	{
		UE_LOG(LogTemp, Error, TEXT("UResourceGeneratorCard::PlayCard_Implementation - Structure not ResourceCollector! Failed to InitResourceCollector()"))
	}
	

	UGameplayStatics::FinishSpawningActor(Structure, Structure->ActorTransform);
	UE_LOG(LogTemp, Display, TEXT("UResourceGeneratorCard::PlayCard_Implementation - Finished spawning structure!"))
	
	// If successful, return true
	return true;
}
