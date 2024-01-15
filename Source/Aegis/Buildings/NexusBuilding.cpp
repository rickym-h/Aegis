// Fill out your copyright notice in the Description page of Project Settings.


#include "NexusBuilding.h"

#include "BuildingHealthComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ANexusBuilding::ANexusBuilding()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>("Building Mesh");
	BuildingMesh->SetupAttachment(RootComponent);
	
	HealthComponent = CreateDefaultSubobject<UBuildingHealthComponent>("Health Component");
}

// Called when the game starts or when spawned
void ANexusBuilding::BeginPlay()
{
	Super::BeginPlay();

	// FTimerHandle DealDamageTimerHandle;
	//
	// FTimerDelegate Delegate; // Delegate to bind function with parameters
	// Delegate.BindUFunction(this, "TakeDamage", 10); // Character is the parameter we wish to pass with the function.
	//
	// GetWorld()->GetTimerManager().SetTimer(DealDamageTimerHandle, Delegate, 5, true);

	UGameplayStatics::ApplyDamage(this, 10, GetWorld()->GetFirstPlayerController(), this, UDamageType::StaticClass());
}

// Called every frame
void ANexusBuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

