// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Structures/Towers/Tower.h"
#include "BombTower.generated.h"

class AEnemy;
class UProjectileComponent;
class UDefenderRangeComponent;

UCLASS()
class AEGIS_API ABombTower : public ATower
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABombTower();

	void InitBombTowerData(const int InRangeInMeters, const float InShotsPerSecond, const float InDamage, const float InExplosionRadius);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
	UDefenderRangeComponent* RangeComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
	UProjectileComponent* ProjectileComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Bomb Tower Data")
	float ShotsPerSecond;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Bomb Tower Data")
	float Damage;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Bomb Tower Data")
	float ExplosionRadius;
	
	UFUNCTION()
	void TryFireAtEnemy(const AEnemy* Enemy);
	bool bShotAvailable = true;
	
	UFUNCTION()
	void ReloadShot();
	
	UPROPERTY()
	FTimerHandle ReloadTimerHandle;
};
