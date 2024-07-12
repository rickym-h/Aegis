// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Pawns/ResourcesData.h"
#include "Aegis/Structures/Structure.h"
#include "ResourceCollector.generated.h"

/**
 * 
 */
UCLASS()
class AEGIS_API AResourceCollector : public AStructure
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable)
	void GenerateResources();
	
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Structure Data")
	UResourcesData* ResourcesToAdd;
};
