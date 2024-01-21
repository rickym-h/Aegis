// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Map/AegisMap.h"
#include "GameFramework/Pawn.h"
#include "BaseEnemy.generated.h"


class AAegisGameStateBase;

UCLASS()
class AEGIS_API ABaseEnemy : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABaseEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Mesh")
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FTileCoord FromTile;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FTileCoord GoalTile;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MovementSpeed = 1.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	const AAegisGameStateBase* GameState;
	
	float DistanceToGoalTile() const;
	float DistanceToGoalTile(const FVector& StartPos) const;
	UFUNCTION()
	void MoveTowardsGoal(float DeltaTime);
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
