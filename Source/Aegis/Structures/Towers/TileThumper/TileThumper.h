// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Structures/Towers/Tower.h"
#include "TileThumper.generated.h"

class AEnemy;
class UTileRangeComponent;

UCLASS()
class AEGIS_API ATileThumper : public ATower
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATileThumper();

	void InitTileThumper(const float InAttackSpeed, const TSet<FTileCoord> TileRangeOffsets);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
	UTileRangeComponent* RangeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="TileThumper Tower Data")
	float AttackSpeed = 0.f;
	
	UFUNCTION()
	void TryThump(AEnemy* Enemy);
	bool bCanThump = true;
	UFUNCTION()
	void RechargeThump();
	UPROPERTY()
	FTimerHandle RechargeTimerHandle;
};
