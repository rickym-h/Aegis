// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Structures/Towers/Tower.h"
#include "Aegis/Utilities/ProjectileManager.h"
#include "ProjectileTower.generated.h"

class AEnemy;
class UDefenderRangeComponent;
/**
 * 
 */

UCLASS()
class AEGIS_API AProjectileTower : public ATower
{
	GENERATED_BODY()

	AProjectileTower();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile Tower Data")
	int TowerRange;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UDefenderRangeComponent* RangeComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UStaticMeshComponent* PedestalMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UStaticMeshComponent* PointAtTargetMesh;


	UFUNCTION(BlueprintCallable)
	void TryFireAtEnemy(AEnemy* Enemy);

	UFUNCTION(BlueprintCallable)
	void FireProjectileAtEnemy(AEnemy* Enemy);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile Tower Data")
	bool ShotAvailable;

	UFUNCTION()
	void ReloadShot();

	UPROPERTY()
	FTimerHandle ReloadTimerHandle;

public:
	void SetTowerRange(const int NewRange);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile Data")
	float AttackSpeed;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile Data")
	UStaticMesh* ProjectileMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile Data")
	float ProjectileSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile Data")
	FProjectileDamagePackage DamagePackage;
};
