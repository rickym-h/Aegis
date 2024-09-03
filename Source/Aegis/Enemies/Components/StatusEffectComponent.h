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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DurationSeconds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpeedMultiplier;

	bool operator==(const FSlowEffect& Other) const
	{
		return (this->DurationSeconds ==  Other.DurationSeconds && this->SpeedMultiplier ==  Other.SpeedMultiplier);
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpeedMultiplierChangedSignature, float, NewSpeedMultiplier);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AEGIS_API UStatusEffectComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UStatusEffectComponent();

	FOnSpeedMultiplierChangedSignature OnSpeedMultiplierChangedDelegate;

	void ApplySlowEffect(FSlowEffect SlowEffect);

	float GetSpeedMultiplier() const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	TArray<FSlowEffect> SlowEffects;

	UFUNCTION()
	void RemoveSlowEffect(FSlowEffect SlowEffect);
};
