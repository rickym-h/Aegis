// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileComponent.h"

#include "Aegis/Core/GameStates/AegisGameStateBase.h"
#include "Aegis/Utilities/ProjectileManager.h"

// Sets default values for this component's properties
UProjectileComponent::UProjectileComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UProjectileComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UProjectileComponent::FireArrowAtEnemy(const FVector& StartPoint, const AEnemy* Enemy, const float Damage)
{
	const AAegisGameStateBase* GameState = Cast<AAegisGameStateBase>(Enemy->GetWorld()->GetGameState());
	
	if (!GameState)
	{
		UE_LOG(LogTemp, Error, TEXT("UProjectileComponent::FireArrowAtEnemy - GameState not found!!"))
		return;
	}

	FProjectileDamagePackage DamagePackage = FProjectileDamagePackage();
	DamagePackage.PhysicalDamage = Damage;

	UStaticMesh* ArrowMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Script/Engine.StaticMesh'/Game/Aegis/Art/VoxelArt/SM_Arrow.SM_Arrow'"));
	GameState->ProjectileManager->FireProjectile(DamagePackage, StartPoint, Enemy, 10, ArrowMesh);
}

void UProjectileComponent::FireBombProjectileAtEnemy(const FVector& StartPoint, const AEnemy* Enemy, const float Damage, const float ExplosionRadius)
{
	const AAegisGameStateBase* GameState = Cast<AAegisGameStateBase>(Enemy->GetWorld()->GetGameState());
	
	if (!GameState)
	{
		UE_LOG(LogTemp, Error, TEXT("UProjectileComponent::FireArrowAtEnemy - GameState not found!!"))
		return;
	}

	FProjectileDamagePackage DamagePackage = FProjectileDamagePackage();
	DamagePackage.PhysicalDamage = Damage;
	DamagePackage.ExplosionRadius = ExplosionRadius;

	UStaticMesh* ArrowMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Script/Engine.StaticMesh'/Game/Aegis/Art/VoxelArt/SM_Arrow.SM_Arrow'"));
	GameState->ProjectileManager->FireProjectile(DamagePackage, StartPoint, Enemy, 10, ArrowMesh);
}
