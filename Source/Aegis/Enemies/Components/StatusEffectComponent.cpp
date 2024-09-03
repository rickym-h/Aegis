// Fill out your copyright notice in the Description page of Project Settings.


#include "StatusEffectComponent.h"


// Sets default values for this component's properties
UStatusEffectComponent::UStatusEffectComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UStatusEffectComponent::ApplySlowEffect(const FSlowEffect SlowEffect)
{
	if (SlowEffect.DurationSeconds <= 0.0f)
	{
		return;
	}

	const float PreSlowSpeedMultiplier = GetSpeedMultiplier();
	
	// Add the slow effect to the array
	SlowEffects.Add(SlowEffect);

	if (const float NewSpeedMultiplier = GetSpeedMultiplier(); NewSpeedMultiplier != PreSlowSpeedMultiplier)
	{
		OnSpeedMultiplierChangedDelegate.Broadcast(NewSpeedMultiplier);
	}

	// Create a timer to remove the slow effect after the duration
	FTimerHandle TimerHandle;
	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, "RemoveSlowEffect", SlowEffect);
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		Delegate,
		SlowEffect.DurationSeconds,
		false);
}

float UStatusEffectComponent::GetSpeedMultiplier() const
{
	float LowestSpeedMultiplier = 1.f;

	for (const auto& SlowEffect : SlowEffects)
	{
		LowestSpeedMultiplier = FMath::Min(LowestSpeedMultiplier, SlowEffect.SpeedMultiplier);
	}

	return LowestSpeedMultiplier;
}


// Called when the game starts
void UStatusEffectComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UStatusEffectComponent::RemoveSlowEffect(const FSlowEffect SlowEffect)
{
	const float PreSlowSpeedMultiplier = GetSpeedMultiplier();
	
	SlowEffects.Remove(SlowEffect);
	
	if (const float NewSpeedMultiplier = GetSpeedMultiplier(); NewSpeedMultiplier != PreSlowSpeedMultiplier)
	{
		OnSpeedMultiplierChangedDelegate.Broadcast(NewSpeedMultiplier);
	}
}
