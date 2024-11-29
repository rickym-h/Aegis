// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ProjectileCallbackInterface.generated.h"

class AEnemy;
// This class does not need to be modified.
UINTERFACE()
class UProjectileCallbackInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AEGIS_API IProjectileCallbackInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interface Functions")
	void SingleTargetProjectileCallback(AEnemy* HitEnemy, const float DamageApplied, const FHitResult& HitResult);
	
	// UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interface Functions")
	// void RadialProjectileCallback();
};
