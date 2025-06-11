// Fill out your copyright notice in the Description page of Project Settings.


#include "StatusEffectComponent.h"

#include "Aegis/Enemies/Damage/MagicDamageType.h"
#include "Kismet/GameplayStatics.h"


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

void UStatusEffectComponent::ApplyPoisonStacks(const int32 Stacks, const float DurationSeconds)
{
	PoisonStacks += Stacks;

	FTimerHandle RemovePoisonStacksTimerHandle;
	FTimerDelegate Delegate; // Delegate to bind function with parameters
	Delegate.BindUFunction(this, "RemovePoisonStacks", Stacks); // Character is the parameter we wish to pass with the function.

	GetWorld()->GetTimerManager().SetTimer(RemovePoisonStacksTimerHandle, Delegate, DurationSeconds, false);

	if (PoisonStacks == Stacks)
	{
		// This stack is first poison applied from nothing, start damage.
		GlobalApplyPoisonTimerHandle = FTimerHandle();
		
		GetWorld()->GetTimerManager().SetTimer(
			GlobalApplyPoisonTimerHandle, // handle to cancel timer at a later time
			this, // the owning object
			&UStatusEffectComponent::DealPoisonDamage, // function to call on elapsed
			1, // float delay until elapsed
			true); // looping?
	}
}

void UStatusEffectComponent::RemovePoisonStacks(const int32 Stacks)
{
	PoisonStacks = FMath::Max(0, PoisonStacks - Stacks);
	if (PoisonStacks == 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(GlobalApplyPoisonTimerHandle);
	}
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
	
	SlowEffects.RemoveSingle(SlowEffect);
	
	if (const float NewSpeedMultiplier = GetSpeedMultiplier(); NewSpeedMultiplier != PreSlowSpeedMultiplier)
	{
		OnSpeedMultiplierChangedDelegate.Broadcast(NewSpeedMultiplier);
	}
}

void UStatusEffectComponent::DealPoisonDamage()
{
	UE_LOG(LogTemp, Warning, TEXT("UStatusEffectComponent::DealPoisonDamage() - Applying poison damage: %i"), PoisonStacks)
	UGameplayStatics::ApplyDamage(GetOwner(), PoisonStacks, GetWorld()->GetFirstPlayerController(), nullptr, UMagicDamageType::StaticClass());
}
