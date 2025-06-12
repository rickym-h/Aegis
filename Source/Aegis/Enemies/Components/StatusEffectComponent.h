// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatusEffectComponent.generated.h"

USTRUCT(Blueprintable)
struct FSlowEffect
{
	GENERATED_BODY()

	FSlowEffect()
	{
		DurationSeconds = 0.f;
		SpeedMultiplier = 1.f;
	}

	FSlowEffect(const float InDurationSeconds, const float InSpeedMultiplier)
	{
		DurationSeconds = InDurationSeconds;
		SpeedMultiplier = InSpeedMultiplier;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DurationSeconds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpeedMultiplier;

	bool operator==(const FSlowEffect& Other) const
	{
		return (this->DurationSeconds ==  Other.DurationSeconds && this->SpeedMultiplier ==  Other.SpeedMultiplier);
	}
};
USTRUCT(Blueprintable)
struct FPoisonEffect
{
	GENERATED_BODY()

	FPoisonEffect()
	{
		DurationSeconds = 0.f;
		PoisonStacks = 1.f;
	}

	FPoisonEffect(const float InDurationSeconds, const float InPoisonStacks)
	{
		DurationSeconds = InDurationSeconds;
		PoisonStacks = InPoisonStacks;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DurationSeconds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PoisonStacks;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpeedMultiplierChangedSignature, float, NewSpeedMultiplier);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AEGIS_API UStatusEffectComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UStatusEffectComponent();

	// Slows
	FOnSpeedMultiplierChangedSignature OnSpeedMultiplierChangedDelegate;
	void ApplySlowEffect(const FSlowEffect SlowEffect);
	float GetSpeedMultiplier() const;

	// Poison
	void ApplyPoisonStacks(const FPoisonEffect PoisonEffect);
	
	UFUNCTION()
	void RemovePoisonStacks(const int32 &Stacks);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Slows
	TArray<FSlowEffect> SlowEffects;
	UFUNCTION()
	void RemoveSlowEffect(FSlowEffect SlowEffect);

	// Poison
	UPROPERTY()
	int32 PoisonStacks;
	FTimerHandle GlobalApplyPoisonTimerHandle;
	UFUNCTION()
	void DealPoisonDamage();
};
