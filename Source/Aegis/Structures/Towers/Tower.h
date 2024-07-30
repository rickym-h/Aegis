// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Structures/Structure.h"
#include "GameFramework/Actor.h"
#include "Tower.generated.h"

UCLASS()
class AEGIS_API ATower : public AStructure
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATower();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Tower Data")
	USceneComponent* SourcePoint;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Decals")
	UDecalComponent* RangeIndicatorDecal;
	
};
