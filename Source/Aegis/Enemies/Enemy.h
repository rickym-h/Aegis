// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Map/AegisMap.h"
#include "Engine/DataTable.h"
#include "Enemy.generated.h"

class UPaperFlipbookComponent;
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	USceneComponent* FloorLevel;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	UCapsuleComponent* CollisionCapsuleComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	UPaperFlipbookComponent* FlipbookComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	bool bShouldFlip;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	UStatusEffectComponent* StatusEffectComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FTileCoord FromTile;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FTileCoord GoalTile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy Data")
	float MovementSpeed = 200.f;
	float CurrentMovementSpeed;

	UFUNCTION()
	void OnSpeedMultiplierChanged(float NewSpeedMultiplier);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy Data")
	float DamageToNexus = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy Data")
	float MaxHealth;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Enemy Data")
	float Health = MaxHealth;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy Data")
	float MaxArmour;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Enemy Data")
	float Armour = MaxArmour;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy Data")
	float MaxMagicResist;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Enemy Data")
	float MagicResist = MaxMagicResist;

	

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


USTRUCT(BlueprintType)
struct FEnemyData : public FTableRowBase
{
	GENERATED_BODY()

	/** Name of Enemy */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Generator")
	FString Enemy = "DEFAULT ENEMY NAME";

	/** Point Cost of Enemy */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Generator")
	int32 Points = 1;

	/** Enemy Tooltip */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Generator")
	FString TooltipText = "DEFAULT TOOLTIP TEXT";

	/** Generator Image */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Generator")
	TSubclassOf<AEnemy> ActorToSpawn;
};