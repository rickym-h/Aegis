// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Cards/StructureCards/ProjectileTowerCard/ProjectileTowerCard.h"
#include "Aegis/Enemies/Components/StatusEffectComponent.h"
#include "SlowingProjectileTowerCard.generated.h"

/**
 * 
 */
UCLASS()
class AEGIS_API USlowingProjectileTowerCard : public UProjectileTowerCard
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slowing Projectile Tower Card Data")
	FSlowEffect EffectToApply;

protected:
	virtual bool PlayCard_Implementation(const FTileCoord& LocationCoord) override;
};
