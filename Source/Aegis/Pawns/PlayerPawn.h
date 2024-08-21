// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Pawn.h"
#include "PlayerPawn.generated.h"

class AStructure;
class UResourcesData;
class UFloatingPawnMovement;
class AAegisGameStateBase;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class AEGIS_API APlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Game")
	AAegisGameStateBase* GameState;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
	UFloatingPawnMovement* MovementComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
	UCameraComponent* Camera;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
	USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* FocusPoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	class UInputMappingContext* InputMapping;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	class UInputConfigData* InputActions;

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Placing Structures")
	TArray<UStaticMeshComponent*> StructureHolograms;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Decals")
	UMaterial* DecalMaterial;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Decals")
	UDecalComponent* RangeIndicatorDecal;

	int BoomArmTargetLength;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Move(const FInputActionValue& InputActionValue);
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
