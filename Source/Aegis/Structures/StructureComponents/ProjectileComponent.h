// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Enemies/Enemy.h"
#include "Aegis/Utilities/ProjectileManager.h"
#include "Components/ActorComponent.h"
#include "ProjectileComponent.generated.h"


UENUM()
enum EProjectileType
{
	ArcArrow UMETA(DisplayName = "Arcing Arrow"),
	RunicSpark UMETA(DisplayName = "Runic Spark"),
	CustomArcProjectile UMETA(DisplayName = "Custom Arc Projectile"),
	CustomHomingProjectile UMETA(DisplayName = "Custom Homing Projectile"),
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AEGIS_API UProjectileComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UProjectileComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY()
	AAegisGameStateBase* GameState;

public:
	UFUNCTION()
	void FireArcArrow(const FVector& StartPoint, const AEnemy* Enemy, const float Damage) const;
	UFUNCTION()
	void FireRunicSpark(const FVector& StartPoint, const AEnemy* Enemy, const float Damage) const;
	UFUNCTION()
	void FireCustomArcProjectile(const FVector& StartPoint, const FVector& CentreControlPoint, const FVector& EndPoint, const FProjectileDamagePackage DamagePackage, float
	                             ProjectileSpeed, UStaticMesh* ProjectileMesh) const;
};
