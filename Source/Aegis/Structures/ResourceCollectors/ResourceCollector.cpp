// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourceCollector.h"

#include "Aegis/Core/GameStates/AegisGameStateBase.h"
#include "Aegis/Enemies/EnemyFactory.h"
#include "Aegis/Pawns/AegisPlayerController.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AResourceCollector::AResourceCollector()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>("BuildingMesh");
	BuildingMesh->SetupAttachment(MeshOrigin);
}

// Called when the game starts or when spawned
void AResourceCollector::BeginPlay()
{
	Super::BeginPlay();

	if (AAegisGameStateBase* GameState = Cast<AAegisGameStateBase>(GetWorld()->GetGameState()))
	{
		GameState->EnemyFactory->OnWaveEndDelegate.AddUniqueDynamic(this, &AResourceCollector::GenerateWaveEndResources);
	}
}

void AResourceCollector::InitResourceCollector(const FResources& InWaveEndResourcesGenerated)
{
	this->WaveEndResourcesGenerated = InWaveEndResourcesGenerated;
}

void AResourceCollector::GenerateWaveEndResources()
{
	UE_LOG(LogTemp, Display, TEXT("Generating wave end resources"));
	if (const AAegisPlayerController* PlayerController = Cast<AAegisPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
	{
		PlayerController->Resources->AddResources(WaveEndResourcesGenerated);
	}
}