// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Enemies/Enemy.h"
#include "Aegis/Structures/Towers/Tower.h"
#include "ArcherTower.generated.h"

class UProjectileComponent;
class UDefenderRangeComponent;
/**
 * 
 */
UCLASS()
class AEGIS_API AArcherTower : public ATower
{
	GENERATED_BODY()

public:
	AArcherTower();

	void InitArcherTowerData(const int InRangeInMeters, const float InShotsPerSecond, const float InDamage);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
	UDefenderRangeComponent* RangeComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
	UProjectileComponent* ProjectileComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Archer Tower Data")
	float ShotsPerSecond;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Archer Tower Data")
	float Damage;

	virtual void BeginPlay() override;


	UFUNCTION()
	void TryFireAtEnemy(const AEnemy* Enemy);
	
	bool bShotAvailable = true;
	UFUNCTION()
	void ReloadShot();
	UPROPERTY()
	FTimerHandle ReloadTimerHandle;
};
