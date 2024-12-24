// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Enemies/Components/StatusEffectComponent.h"
#include "Aegis/Structures/Interfaces/ProjectileCallbackInterface.h"
#include "Aegis/Structures/Towers/ProjectileTower/ProjectileTower.h"
#include "SlowingProjectileTower.generated.h"

UCLASS()
class AEGIS_API ASlowingProjectileTower : public AProjectileTower, public IProjectileCallbackInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASlowingProjectileTower();

	void InitSlowingProjectileTower(const FProjectileDamagePackage InProjectileDamagePackage, const float InAttackSpeed, const int32 InRangeTiles, UStaticMesh* InProjectileMesh, FSlowEffect InEffectToApply);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
	FSlowEffect EffectToApply;

public:

	virtual void SingleTargetProjectileCallback_Implementation(AEnemy* HitEnemy, const float DamageApplied, const FHitResult& HitResult) override;
	
};
