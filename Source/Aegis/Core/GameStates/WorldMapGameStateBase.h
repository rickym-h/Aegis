// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AegisGameStateBase.h"
#include "WorldMapGameStateBase.generated.h"

class UAegisWorldMap;
/**
 * 
 */
UCLASS()
class AEGIS_API AWorldMapGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Map")
	UAegisWorldMap* WorldMap;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Factories")
	UAegisMapFactory* MapFactory;
	
	UFUNCTION(BlueprintCallable)
	void GenerateWorldMapFromMapFactory();
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Factory Classes")
	TSubclassOf<UAegisMapFactory> MapFactoryClass;
};
