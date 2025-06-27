// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Structures/Towers/Tower.h"
#include "LaserTower.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
class AEnemy;
class UTileRangeComponent;

UCLASS()
class AEGIS_API ALaserTower : public ATower
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALaserTower();

	void TryDealDamage();
	void InitLaserTower(const int32 InRangeTiles, const float InDamagePerSecond, const float InDamageFireRate);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
	UTileRangeComponent* TileRangeComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	UNiagaraComponent* LaserParticleComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Tower Stats")
	float DamageFireRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Tower Stats")
	float DamagePerSecond;
	
	UFUNCTION()
	void TryTargetNewEnemy(AEnemy* TargetEnemy);
	UFUNCTION()
	void TryStopTargetEnemy(AEnemy* TargetEnemy);

	UPROPERTY()
	AEnemy* TargetedEnemy;
};
