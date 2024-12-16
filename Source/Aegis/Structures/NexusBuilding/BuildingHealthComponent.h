// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuildingHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealthDepletedSignature);

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
	float MaxHealth = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health")
	float CurrentHealth = MaxHealth;

public:
	UFUNCTION()
	void TakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	int GetCurrentHealth();

	UFUNCTION()
	void SetCurrentHealth(int Health);

	UPROPERTY(BlueprintAssignable)
	FOnHealthDepletedSignature OnHealthDepletedDelegate;
};
