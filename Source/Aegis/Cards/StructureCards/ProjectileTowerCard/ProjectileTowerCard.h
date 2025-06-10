// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Cards/StructureCard.h"
#include "Aegis/Cards/Interfaces/PlayableCardInterface.h"
#include "Aegis/Cards/Interfaces/TileRangeInterface.h"
#include "Aegis/Structures/StructureComponents/ProjectileComponent.h"
#include "Aegis/Utilities/ProjectileManager.h"
#include "ProjectileTowerCard.generated.h"

/**
 * 
 */
UCLASS(HideDropDown)
class AEGIS_API UProjectileTowerCard : public UStructureCard, public IPlayableCardInterface, public ITileRangeInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile Details")
	TEnumAsByte<EProjectileType>  ProjectileType = EProjectileType::ArcArrow;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Details|Custom Projectile", meta=(EditCondition="ProjectileType == EProjectileType::CustomArcProjectile || ProjectileType == EProjectileType::CustomHomingProjectile", EditConditionHides))
	FProjectileDamagePackage ProjectileDamagePackage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Details|Custom Projectile", meta=(EditCondition="ProjectileType == EProjectileType::CustomArcProjectile || ProjectileType == EProjectileType::CustomHomingProjectile", EditConditionHides))
	UStaticMesh* ProjectileMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Details")
	float AttackSpeed = 0.f; // Higher is faster
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Tower Card Data")
	int32 RangeTiles = 0;

protected:
	virtual bool PlayCard_Implementation(const FTileCoord& LocationCoord) override;
	virtual TSet<FTileCoord> GetRangeTileOffsets_Implementation() override;
	virtual TSet<FTileCoord> RotateTileRangeOffsets_Implementation(const bool bClockwise) override;
};
