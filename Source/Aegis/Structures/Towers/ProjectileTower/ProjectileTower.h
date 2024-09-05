// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Structures/Towers/Tower.h"
#include "Aegis/Utilities/ProjectileManager.h"
#include "ProjectileTower.generated.h"

class UDefenderRangeComponent;
class UProjectileComponent;

UCLASS()
class AEGIS_API AProjectileTower : public ATower
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProjectileTower();

	void InitProjectileTower(const FProjectileDamagePackage InProjectileDamagePackage, const float InAttackSpeed, const float InRangeMetres, UStaticMesh* InProjectileMesh);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
	UDefenderRangeComponent* RangeComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
	UProjectileComponent* ProjectileComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Projectile Tower Data")
	UStaticMesh* ProjectileMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Projectile Tower Data")
	FProjectileDamagePackage ProjectileDamagePackage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Projectile Tower Data")
	float AttackSpeed = 0.f;
	
	UFUNCTION()
	void TryFireAtEnemy(const AEnemy* Enemy);
	bool bShotAvailable = true;
	UFUNCTION()
	void ReloadShot();
	UPROPERTY()
	FTimerHandle ReloadTimerHandle;

};
