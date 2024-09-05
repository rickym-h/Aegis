// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Map/AegisMap.h"
#include "GameFramework/Pawn.h"
#include "Enemy.generated.h"

class UStatusEffectComponent;
class UCapsuleComponent;
class USphereComponent;
class AAegisGameStateBase;

UCLASS()
class AEGIS_API AEnemy : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEnemy();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	float GetDistanceToNexus() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Tower Data")
	USceneComponent* TargetPoint;
	
	UFUNCTION(BlueprintCallable)
	float GetHealthAsPercentage() const;

	UStatusEffectComponent* GetStatusEffectComponent() const;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Mesh")
	UCapsuleComponent* CollisionCapsuleComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Mesh")
	UStaticMeshComponent* Mesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	UStatusEffectComponent* StatusEffectComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FTileCoord FromTile;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FTileCoord GoalTile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy Data")
	float MovementSpeed = 200.f;
	float CurrentMovementSpeed = MovementSpeed;

	UFUNCTION()
	void OnSpeedMultiplierChanged(float NewSpeedMultiplier);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy Data")
	float DamageToNexus = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy Data")
	float MaxHealth;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Enemy Data")
	float Health = MaxHealth;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	const AAegisGameStateBase* GameState;

	float DistanceToGoalTile() const;
	float DistanceToGoalTile(const FVector& StartPos) const;
	UFUNCTION()
	void MoveTowardsGoal(float DeltaTime);

	UFUNCTION()
	void OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	                  bool bFromSweep, const FHitResult& SweepResult);
};
