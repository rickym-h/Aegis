// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NexusBuilding.generated.h"

class UCapsuleComponent;
class UBuildingHealthComponent;

UCLASS()
class AEGIS_API ANexusBuilding : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ANexusBuilding();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
	UBuildingHealthComponent* HealthComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	UCapsuleComponent* CollisionCapsule;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* BuildingMesh;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
