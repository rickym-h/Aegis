// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "ProjectileManager.generated.h"

class AEnemy;

USTRUCT(Blueprintable)
struct FProjectileDamagePackage
{
	GENERATED_BODY()

	FProjectileDamagePackage()
	{
		this->PhysicalDamage = 0;
		this->ExplosionRadius = 0;
		this->OnHitParticleSystem = nullptr;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage")
	float PhysicalDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage")
	float ExplosionRadius;

	UPROPERTY(EditAnywhere, Category = "Firing")
	UNiagaraSystem* OnHitParticleSystem;
};

USTRUCT(Blueprintable)
struct FProjectilePackage
{
	FProjectilePackage()
	{
		DamagePackage = FProjectileDamagePackage();
		ResponsibleSource = nullptr;
		StartPoint = FVector::ZeroVector;
		ForwardVector = FVector::ZeroVector;
		TargetEnemy = nullptr;
		Speed = 0;
		WorldTimeSeconds = 0;
	}
	
	FProjectilePackage(const FProjectileDamagePackage& InDamagePackage, UObject* InResponsibleSource, const FVector& InStartPoint, const AEnemy* InTargetEnemy, const float InSpeed, const int InWorldTimeSeconds)
	{
		DamagePackage = InDamagePackage;
		ResponsibleSource = InResponsibleSource;
		StartPoint = InStartPoint;
		ForwardVector = FVector::ZeroVector;
		TargetEnemy = InTargetEnemy;
		Speed = InSpeed;
		WorldTimeSeconds = InWorldTimeSeconds;
	}

	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile Data")
	FProjectileDamagePackage DamagePackage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile Data")
	UObject* ResponsibleSource; 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile Data")
	FVector StartPoint;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile Data")
	const AEnemy* TargetEnemy;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile Data")
	float Speed;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile Data")
	FVector ForwardVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile Data")
	int32 WorldTimeSeconds;
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

	UFUNCTION()
	void OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitResult);
	UStaticMeshComponent* FireProjectile(const FProjectileDamagePackage DamagePackage, UObject* ResponsibleSource, const FVector& Start, const AEnemy* TargetEnemy, const float Speed, UStaticMesh* ProjectileMesh);

protected:
	UStaticMeshComponent* AcquireProjectile(const FProjectilePackage& ProjectilePackage);

	void ReleaseProjectile(UStaticMeshComponent* Projectile);

	void CleanProjectile(UStaticMeshComponent* Projectile);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectiles")
	TArray<UStaticMeshComponent*> AvailableProjectiles;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectiles")
	TMap<UStaticMeshComponent*, FProjectilePackage> ActiveProjectiles;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectiles")
	TArray<UStaticMeshComponent*> ProjectilesToRelease;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Root")
	USceneComponent* SceneRootComponent;
};
