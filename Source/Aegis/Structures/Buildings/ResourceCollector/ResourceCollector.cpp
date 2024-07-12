// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourceCollector.h"

#include "Aegis/AegisGameStateBase.h"
#include "Aegis/Enemies/EnemyFactory.h"
#include "Aegis/Pawns/PlayerPawn.h"
#include "Aegis/Pawns/ResourcesData.h"
#include "Kismet/GameplayStatics.h"

void AResourceCollector::GenerateResources()
{
	const APlayerPawn* Player = Cast<APlayerPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	Player->Resources->AddResources(ResourcesToAdd);
}

void AResourceCollector::BeginPlay()
{
	Super::BeginPlay();
	
	ResourcesToAdd = NewObject<UResourcesData>(GetWorld(), UResourcesData::StaticClass());
	ResourcesToAdd->AddResources(1,1,1,1,1);

	const AAegisGameStateBase* GameState = Cast<AAegisGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
	GameState->EnemyFactory->OnWaveEndDelegate.AddUniqueDynamic(this, &AResourceCollector::GenerateResources);
}
