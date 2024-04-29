// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ProjectileManager.generated.h"

USTRUCT(Blueprintable)
struct FProjectileDamagePackage
{
	FProjectileDamagePackage()
	{
		PhysicalDamage = 0.f;
	}

	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Damage")
	float PhysicalDamage;
	
};

USTRUCT(Blueprintable)
struct FProjectilePackage
{
	FProjectilePackage()
	{
		DamagePackage = FProjectileDamagePackage();
		StartPoint = FVector::ZeroVector;
		EndPoint =  FVector::ZeroVector;
		Speed = 0;
	}
	
	FProjectilePackage(const FProjectileDamagePackage& InDamagePackage, const FVector& InStartPoint, const FVector& InEndPoint, const float InSpeed)
	{
		DamagePackage = InDamagePackage;
		StartPoint = InStartPoint;
		EndPoint = InEndPoint;
		Speed = InSpeed;
	}

	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile Data")
	FProjectileDamagePackage DamagePackage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile Data")
	FVector StartPoint;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile Data")
	FVector EndPoint;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile Data")
	float Speed;
	// TODO float ArcHeight;
};

/**
 * 
 */
UCLASS()
class AEGIS_API AProjectileManager : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this pawn's properties
	AProjectileManager();

	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	
	UStaticMeshComponent* FireProjectile(const FProjectileDamagePackage DamagePackage, const FVector& Start, const FVector& End, const float Speed, UStaticMesh* ProjectileMesh);

protected:
	UStaticMeshComponent* AcquireProjectile(const FProjectilePackage& ProjectilePackage);

	void ReleaseProjectile(UStaticMeshComponent* Projectile);

	void CleanProjectile(UStaticMeshComponent* Projectile);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectiles")
	TArray<UStaticMeshComponent*> AvailableProjectiles;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectiles")
	TMap<UStaticMeshComponent*, FProjectilePackage> ActiveProjectiles;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Root")
	USceneComponent* SceneRootComponent;
};
