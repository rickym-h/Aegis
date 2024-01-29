// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseTower.h"
#include "BaseProjectileTower.generated.h"

/**
 * 
 */
UCLASS()
class AEGIS_API ABaseProjectileTower : public ABaseTower
{
	GENERATED_BODY()
	
public:
	ABaseProjectileTower();

protected:
	virtual void BeginPlay() override;
	
	
};
