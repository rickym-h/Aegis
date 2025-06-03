// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Structures/Towers/Tower.h"
#include "Aegis/Utilities/ProjectileManager.h"
#include "Aegis/Structures/StructureComponents/ProjectileComponent.h"
#include "ProjectileTower.generated.h"

class UTileRangeComponent;
class UProjectileComponent;

UCLASS()
class AEGIS_API AProjectileTower : public ATower
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProjectileTower();

	void InitProjectileTower(EProjectileType InProjectileType, const FProjectileDamagePackage InProjectileDamagePackage, UStaticMesh* InProjectileMesh, const float InAttackSpeed, const int32 InRangeTiles);

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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Projectile Details")
	float AttackSpeed = 0.f;
	
	UFUNCTION()
	void TryFireAtEnemy(const AEnemy* Enemy);
	bool bShotAvailable = true;
	UFUNCTION()
	void ReloadShot();
	UPROPERTY()
	FTimerHandle ReloadTimerHandle;

};
