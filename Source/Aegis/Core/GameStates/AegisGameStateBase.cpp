// Fill out your copyright notice in the Description page of Project Settings.


#include "AegisGameStateBase.h"

#include "Aegis/Enemies/EnemyFactory.h"
#include "Kismet/GameplayStatics.h"
#include "Aegis/Map/AegisGameMap.h"
#include "Aegis/Map/AegisMapFactory.h"
#include "Aegis/SaveSystem/AegisSaveGame.h"
#include "Aegis/Structures/NexusBuilding/BuildingHealthComponent.h"
#include "Aegis/Structures/NexusBuilding/NexusBuilding.h"
#include "Aegis/Utilities/ProjectileManager.h"


void AAegisGameStateBase::BeginPlay()
{
	Super::BeginPlay();

	if (ProjectileManagerClass)
	{
		ProjectileManager = GetWorld()->SpawnActor<AProjectileManager>(ProjectileManagerClass, FVector::ZeroVector, FRotator::ZeroRotator, FActorSpawnParameters());
	}
	
	GenerateTestMapFromMapFactory();
}

void AAegisGameStateBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	if (MapFactoryClass)
	{
		MapFactory = NewObject<UAegisMapFactory>(GetWorld(), MapFactoryClass);
	}
	
	if (EnemyFactoryClass)
	{
		EnemyFactory = NewObject<UEnemyFactory>(GetWorld(), EnemyFactoryClass);
	}
}

void AAegisGameStateBase::GenerateTestMapFromMapFactory()
{
	// Destroy AegisMap
	if (AegisMap)
	{
		UE_LOG(LogTemp, Error, TEXT("AAegisGameStateBase::GenerateTestMapFromMapFactory - No implementation found to destroy map!"))
	}

	AegisMap = MapFactory->GenerateGameMap(2);
}

void AAegisGameStateBase::SaveGame()
{
	if (!AegisMap)
	{
		UE_LOG(LogTemp, Warning, TEXT("AAegisGameStateBase::SaveGame() - No AegisMap, early return."))
		return;
	}

	if (UAegisSaveGame* SaveGameInstance = Cast<UAegisSaveGame>(UGameplayStatics::CreateSaveGameObject(UAegisSaveGame::StaticClass())))
	{
		// Set up the (optional) delegate.
		FAsyncSaveGameToSlotDelegate SavedDelegate;
		// USomeUObjectClass::SaveGameDelegateFunction is a void function that takes the following parameters: const FString& SlotName, const int32 UserIndex, bool bSuccess
		SavedDelegate.BindUObject(this, &AAegisGameStateBase::SaveGameDelegateFunction);

		// Set data on the savegame object.
		SaveGameInstance->NexusHealth = AegisMap->NexusBuilding->HealthComponent->GetCurrentHealth();

		// Start async save process.
		UGameplayStatics::AsyncSaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex, SavedDelegate);
	}
}

void AAegisGameStateBase::LoadGame()
{
	// Set up the delegate.
	FAsyncLoadGameFromSlotDelegate LoadedDelegate;
	// USomeUObjectClass::LoadGameDelegateFunction is a void function that takes the following parameters: const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData
	LoadedDelegate.BindUObject(this, &AAegisGameStateBase::LoadGameDelegateFunction);
	UGameplayStatics::AsyncLoadGameFromSlot(TEXT("TestSaveSlot"), 0, LoadedDelegate);
}

void AAegisGameStateBase::SaveGameDelegateFunction(const FString& SlotName, const int32 UserIndex, bool bSuccess)
{
	UE_LOG(LogTemp, Warning, TEXT("AAegisGameStateBase::SaveGameDelegateFunction() - Saved game!!"))
}

void AAegisGameStateBase::LoadGameDelegateFunction(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData)
{
	if (const UAegisSaveGame* LoadedSaveData = Cast<UAegisSaveGame>(LoadedGameData))
	{
		if (AegisMap)
		{
			AegisMap->NexusBuilding->HealthComponent->SetCurrentHealth(LoadedSaveData->NexusHealth);
			UE_LOG(LogTemp, Warning, TEXT("AAegisGameStateBase::LoadGameDelegateFunction() - Loaded game!!"))
		}
	}
}
