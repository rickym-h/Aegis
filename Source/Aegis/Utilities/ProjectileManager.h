// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "Aegis/Enemies/Enemy.h"
#include "ProjectileManager.generated.h"

class AEnemy;

USTRUCT(Blueprintable)
struct FProjectileDamagePackage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage")
	float PhysicalDamage = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage")
	float MagicDamage = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage")
	float TrueDamage = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage")
	float ExplosionRadius = 0.f;
};

USTRUCT(Blueprintable)
struct FHomingProjectilePackage
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile Trajectory")
	FVector StartPoint = FVector::ZeroVector;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile Trajectory")
	const AEnemy* TargetEnemy = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile Trajectory")
	float ProjectileSpeed = 0.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile Trajectory")
	float ProjectileRotationSpeed = 0.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile Trajectory")
	double StartLaunchTime = 0.;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile Data")
	int32 MaxHits = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile Data")
	FProjectileDamagePackage DamagePackage = FProjectileDamagePackage();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile Data")
	UObject* ResponsibleSource = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile Particles")
	UNiagaraSystem* OnHitParticleSystem = nullptr; 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile Particles")
	UNiagaraSystem* FlightParticleSystem = nullptr; 
};

USTRUCT(Blueprintable)
struct FArcProjectilePackage
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile Trajectory")
	FVector StartControlPoint = FVector::ZeroVector;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile Trajectory")
	FVector CentreControlPoint = FVector::ZeroVector;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile Trajectory")
	FVector EndControlPoint = FVector::ZeroVector;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile Trajectory")
	float StartToEndTravelSeconds = 0.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile Trajectory")
	double StartLaunchTime = 0.;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile Data")
	FProjectileDamagePackage DamagePackage = FProjectileDamagePackage();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile Data")
	UObject* ResponsibleSource = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile Particles")
	UNiagaraSystem* OnHitParticleSystem = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile Particles")
	UNiagaraSystem* FlightParticleSystem = nullptr; 
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

	UStaticMeshComponent* LaunchHomingProjectile(
		const FVector& StartPoint,
		const AEnemy* TargetEnemy,
		UStaticMesh* Mesh,
		const float ProjectileSpeed,
		const float ProjectileRotationSpeed,
		const int32 MaxHits,
		const FProjectileDamagePackage DamagePackage,
		UObject* ResponsibleSource,
		UNiagaraSystem* OnHitParticleSystem, UNiagaraSystem* FlightParticleSystem);
	
	UStaticMeshComponent* LaunchArcProjectile(
		const FVector& StartControlPoint,
		const FVector& CentreControlPoint,
		const FVector& EndControlPoint,
		UStaticMesh* Mesh,
		const float StartToEndTravelTime,
		const FProjectileDamagePackage DamagePackage,
		UObject* ResponsibleSource,
		UNiagaraSystem* OnHitParticleSystem, UNiagaraSystem* FlightParticleSystem);
	
protected:
	UStaticMeshComponent* AcquireProjectile(const FHomingProjectilePackage& ProjectilePackage);
	UStaticMeshComponent* AcquireProjectile(const FArcProjectilePackage& ProjectilePackage);

	void ReleaseProjectile(UStaticMeshComponent* Projectile);
	void CleanProjectile(UStaticMeshComponent* Projectile) const;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectiles")
	TArray<UStaticMeshComponent*> AvailableProjectiles;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectiles")
	TArray<UStaticMeshComponent*> ProjectilesToRelease;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Active Projectiles")
	TMap<UStaticMeshComponent*, FHomingProjectilePackage> ActiveHomingProjectiles;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Active Projectiles")
	TMap<UStaticMeshComponent*, FArcProjectilePackage> ActiveArcProjectiles;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Root")
	USceneComponent* SceneRootComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Root")
	UNiagaraSystem* BloodHitParticleEffect;

	UFUNCTION()
	void HomingProjectileOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitResult);
	UFUNCTION()
	void ArcProjectileOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitResult);
	void ApplyDamageAndEffects(const UPrimitiveComponent* OverlappedComponent, const FHitResult& HitResult, const UStaticMeshComponent* ProjectileMesh,
							   AEnemy* Enemy,
							   const FProjectileDamagePackage* DamagePackage, UNiagaraSystem* OnHitParticleSystem, UObject* ResponsibleSource);
};
