// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Pawn.h"
#include "PlayerPawn.generated.h"

class UResourcesData;
class UStructureData;
class UFloatingPawnMovement;
class AAegisGameStateBase;
class USpringArmComponent;
class UCameraComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTowersInHandUpdated);


UENUM()
enum EPlayerState
{
	Default UMETA(DisplayName = "Default"),
	Placing UMETA(DisplayName = "Placing Structures")
};

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Game")
	AAegisGameStateBase* GameState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Placing Structures")
	TEnumAsByte<EPlayerState> PlayerActionState;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Placing Structures")
	UStructureData* StructureToPlace;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Placing Structures")
	TArray<UStaticMeshComponent*> StructureHolograms;
	void ClearStructureHolograms();

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateStructureHologramMesh(UStaticMeshComponent* HologramMeshComponent, bool IsValid);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Cards Held")
	TArray<UStructureData*> TowerCardsInHand;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data")
	FHitResult HitResultUnderCursor;
	FHitResult* UpdateHitResultUnderCursor();

	int BoomArmTargetLength;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Click(const FInputActionValue& InputActionValue);
	void Move(const FInputActionValue& InputActionValue);
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(BlueprintAssignable)
	FOnTowersInHandUpdated OnTowersInHandUpdatedDelegate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Resources")
	UResourcesData* Resources;

	UFUNCTION(BlueprintCallable)
	bool AddTowerCardToHand(UStructureData* TowerData);
	UFUNCTION(BlueprintCallable)
	bool RemoveTowerCardFromHand(UStructureData* TowerData);

	UFUNCTION(BlueprintCallable)
	void BeginPlacingStructure(UStructureData* StructureData);
};
