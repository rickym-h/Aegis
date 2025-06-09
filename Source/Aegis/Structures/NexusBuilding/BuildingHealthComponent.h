// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Core/Player/PlayerData.h"
#include "Components/ActorComponent.h"
#include "BuildingHealthComponent.generated.h"

class UAegisGameInstance;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AEGIS_API UBuildingHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBuildingHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health")
	float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health")
	float CurrentHealth;

	UAegisGameInstance* GameInstance;

public:
	UFUNCTION()
	void TakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	int GetCurrentHealth();

	UFUNCTION()
	void SetCurrentHealth(const int Health);

	UPROPERTY(BlueprintAssignable)
	FOnHealthDepletedSignature OnHealthDepletedDelegate;
};
