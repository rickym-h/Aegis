// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Enemies/Components/StatusEffectComponent.h"
#include "Aegis/Structures/Interfaces/ProjectileCallbackInterface.h"
#include "Aegis/Structures/Towers/Tower.h"
#include "Aegis/Utilities/ProjectileManager.h"
#include "Aegis/Structures/StructureComponents/ProjectileComponent.h"
#include "ProjectileTower.generated.h"

class UTileRangeComponent;
class UProjectileComponent;

UCLASS()
class AEGIS_API AProjectileTower : public ATower, public IProjectileCallbackInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProjectileTower();

	void InitProjectileTower(EProjectileType InProjectileType, const FProjectileDamagePackage InProjectileDamagePackage, UStaticMesh* InProjectileMesh, const float InAttackSpeed, const int32 InRangeTiles, const FSlowEffect InSlowEffect, const FPoisonEffect InPoisonEffect);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
	UTileRangeComponent* TileRangeComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
	UProjectileComponent* ProjectileComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Projectile Details")
	TEnumAsByte<EProjectileType>  ProjectileType;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Projectile Details")
	UStaticMesh* ProjectileMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Projectile Details")
	FProjectileDamagePackage CustomProjectileProjectileDamagePackage;

	// Higher is faster
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Projectile Details", meta=(ToolTip="Higher is faster. e.g. 0.5 is once every 2 seconds, 3 is 3 shots per second."))
	float AttackSpeed = 0.f;
	
	UFUNCTION()
	void TryFireAtEnemy(const AEnemy* Enemy);
	bool bShotAvailable = true;
	UFUNCTION()
	void ReloadShot();
	UPROPERTY()
	FTimerHandle ReloadTimerHandle;
	
	FSlowEffect SlowEffect;
	FPoisonEffect PoisonEffect;
	virtual void SingleTargetProjectileCallback_Implementation(AEnemy* HitEnemy, const float DamageApplied, const FHitResult& HitResult) override;

};
