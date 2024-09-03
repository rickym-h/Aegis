// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Cards/PlayableCardInterface.h"
#include "Aegis/Cards/RangeInterface.h"
#include "Aegis/Cards/StructureCard.h"
#include "Aegis/Utilities/ProjectileManager.h"
#include "ProjectileTowerCard.generated.h"

/**
 * 
 */
UCLASS()
class AEGIS_API UProjectileTowerCard : public UStructureCard, public IPlayableCardInterface, public IRangeInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Tower Card Data")
	FProjectileDamagePackage ProjectileDamagePackage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Tower Card Data")
	float AttackSpeed = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Tower Card Data")
	float RangeMetres = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Tower Card Data")
	UStaticMesh* ProjectileMesh;
	

protected:
	virtual bool PlayCard_Implementation(const FTileCoord& LocationCoord) override;
	virtual float GetRangeInMetres_Implementation() override;
};
