// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileTower.h"
#include "BloodrootShrine.generated.h"

UCLASS()
class AEGIS_API ABloodrootShrine : public AProjectileTower
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABloodrootShrine();

	virtual void FireAtEnemy(const AEnemy* Enemy) override;
	
	virtual void SingleTargetProjectileCallback_Implementation(AEnemy* HitEnemy, const float DamageApplied, const FHitResult& HitResult) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
