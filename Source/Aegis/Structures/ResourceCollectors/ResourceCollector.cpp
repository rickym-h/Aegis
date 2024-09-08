// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourceCollector.h"

#include "Aegis/Core/GameStates/AegisGameStateBase.h"
#include "Aegis/Enemies/EnemyFactory.h"
#include "Aegis/Pawns/AegisPlayerController.h"
#include "Aegis/Structures/StructureComponents/MapInterface.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AResourceCollector::AResourceCollector()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>("BuildingMesh");
	BuildingMesh->SetupAttachment(MeshOrigin);
	
	MapInterface = CreateDefaultSubobject<UMapInterface>("MapInterface");
}

// Called when the game starts or when spawned
void AResourceCollector::BeginPlay()
{
	Super::BeginPlay();

	if (const AAegisGameStateBase* GameState = Cast<AAegisGameStateBase>(GetWorld()->GetGameState()))
	{
		GameState->EnemyFactory->OnWaveEndDelegate.AddUniqueDynamic(this, &AResourceCollector::GenerateWaveEndResources);
	}
}

void AResourceCollector::InitResourceCollector(const FResources& InWaveEndResourcesGenerated,
                                               const TMap<TEnumAsByte<EResourceType>, FResources>& InResourcesToGeneratePerAdjacentResource)
{
	this->WaveEndResourcesGenerated = InWaveEndResourcesGenerated;
	this->ResourcesToGeneratePerAdjacentResource = InResourcesToGeneratePerAdjacentResource;
}

void AResourceCollector::GenerateWaveEndResources()
{
	UE_LOG(LogTemp, Display, TEXT("Generating wave end resources"));
	const AAegisPlayerController* PlayerController = Cast<AAegisPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("AResourceCollector::GenerateWaveEndResources - PlayerController is null"));
		return;
	}
	
	PlayerController->Resources->AddResources(WaveEndResourcesGenerated);

	for (TTuple<TEnumAsByte<EResourceType>, FResources> ResourceInfo : ResourcesToGeneratePerAdjacentResource)
	{
		const int32 AdjacentCount = MapInterface->CountResourcesInRange(ResourceInfo.Key, 1, false);
		PlayerController->Resources->AddResources(ResourceInfo.Value * AdjacentCount);
	}
}