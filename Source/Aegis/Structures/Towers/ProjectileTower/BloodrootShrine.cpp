// Fill out your copyright notice in the Description page of Project Settings.


#include "BloodrootShrine.h"

#include "Aegis/Core/GameStates/AegisGameStateBase.h"
#include "Aegis/Map/AegisGameMap.h"
#include "Aegis/Structures/NexusBuilding/BuildingHealthComponent.h"
#include "Aegis/Structures/NexusBuilding/NexusBuilding.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ABloodrootShrine::ABloodrootShrine()
{
}

void ABloodrootShrine::FireAtEnemy(const AEnemy* Enemy)
{
	Super::FireAtEnemy(Enemy);
}

ANexusBuilding* GetNexusBuilding(const UWorld* World) {
    if (!World) return nullptr;
    const AAegisGameStateBase* GameState = Cast<AAegisGameStateBase>(World->GetGameState());
    if (!GameState) return nullptr;
    if (!GameState->AegisMap) return nullptr;
    
    return GameState->AegisMap->NexusBuilding;
}

void ABloodrootShrine::SingleTargetProjectileCallback_Implementation(AEnemy* HitEnemy, const float DamageApplied, const FHitResult& HitResult)
{
	Super::SingleTargetProjectileCallback_Implementation(HitEnemy, DamageApplied, HitResult);
	UE_LOG(LogTemp, Warning, TEXT("ABloodrootShrine::SingleTargetProjectileCallback_Implementation"))

	// Logic for removing HP
	if (ANexusBuilding* NexusBuilding = GetNexusBuilding(GetWorld())) {
		// Apply damage to the Nexus Building
		UGameplayStatics::ApplyDamage(NexusBuilding, NexusBuilding->HealthComponent->GetCurrentHealth()/2, GetWorld()->GetFirstPlayerController(), this, UDamageType::StaticClass());
	}
	
}

// Called when the game starts or when spawned
void ABloodrootShrine::BeginPlay()
{
	Super::BeginPlay();

	// Logic for removing HP
	if (ANexusBuilding* NexusBuilding = GetNexusBuilding(GetWorld())) {
		// Apply damage to the Nexus Building
		UGameplayStatics::ApplyDamage(NexusBuilding, NexusBuilding->HealthComponent->GetCurrentHealth()/2, GetWorld()->GetFirstPlayerController(), this, UDamageType::StaticClass());
	}
}