// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseTower.h"
#include "BaseProjectileTower.generated.h"

class ABaseEnemy;
class UDefenderRangeComponent;
/**
 * 
 */
UCLASS()
class AEGIS_API ABaseProjectileTower : public ABaseTower
{
	GENERATED_BODY()
	
public:
	ABaseProjectileTower();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Defender Stats")
	int TowerRange;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void OnEnemyEnterRange(ABaseEnemy* OtherEnemy);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	UDefenderRangeComponent* RangeComponent;
	
};
