// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingHealthComponent.h"

// Sets default values for this component's properties
UBuildingHealthComponent::UBuildingHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UBuildingHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Owner = GetOwner(); Owner != nullptr)
	{
		Owner->OnTakeAnyDamage.AddDynamic(this, &UBuildingHealthComponent::TakeDamage);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Health Component failed to bind OnTakeAnyDamage for: %s"), *GetOwner()->GetActorNameOrLabel())
	}
}


// Called every frame
void UBuildingHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UBuildingHealthComponent::TakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy,
                                          AActor* DamageCauser)
{
	// Perform logic for damaging building
	this->CurrentHealth -= Damage;
	if (CurrentHealth <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Health brought below 0! Destroying..."))
		if (AActor* Owner = GetOwner())
		{
			// TODO animation and sound effects before destroying
			Owner->Destroy();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Taking Damage! CurrentHealth: %f"), CurrentHealth)
	}
}

int UBuildingHealthComponent::GetCurrentHealth()
{
	return CurrentHealth;
}

void UBuildingHealthComponent::SetCurrentHealth(int Health)
{
	CurrentHealth = Health;
}
