// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingHealthComponent.h"

#include "Aegis/Core/AegisGameInstance.h"
#include "Aegis/Core/Player/PlayerData.h"

// Sets default values for this component's properties
UBuildingHealthComponent::UBuildingHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UBuildingHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	GameInstance = Cast<UAegisGameInstance>(GetWorld()->GetGameInstance());
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Fatal, TEXT("Game Instance is null!"))
		return;
	}
	
	MaxHealth = GameInstance->GetPlayerData()->GetLivesCount();
	CurrentHealth = MaxHealth;
	UE_LOG(LogTemp, Warning, TEXT("CurrentHealth: %f"), CurrentHealth)

	if (AActor* Owner = GetOwner(); Owner != nullptr)
	{
		Owner->OnTakeAnyDamage.AddDynamic(this, &UBuildingHealthComponent::TakeDamage);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Health Component failed to bind OnTakeAnyDamage for: %s"), *GetOwner()->GetActorNameOrLabel())
	}
}

void UBuildingHealthComponent::TakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	// Perform logic for damaging building
	this->CurrentHealth -= Damage;
	OnHealthDepletedDelegate.Broadcast(CurrentHealth);
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
		UE_LOG(LogTemp, Warning, TEXT("%ls: Taking Damage! CurrentHealth: %f"), *DamagedActor->GetName() , CurrentHealth)
	}

	GameInstance->GetPlayerData()->TakeLives(Damage);
}

int UBuildingHealthComponent::GetCurrentHealth()
{
	return CurrentHealth;
}

void UBuildingHealthComponent::SetCurrentHealth(const int Health)
{
	CurrentHealth = Health;
}
