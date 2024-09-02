// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Pawns/ResourcesData.h"
#include "Aegis/Structures/Structure.h"
#include "ResourceCollector.generated.h"

UCLASS()
class AEGIS_API AResourceCollector : public AStructure
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AResourceCollector();

	UFUNCTION()
	void InitResourceCollector(const FResources& InWaveEndResourcesGenerated);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resources")
	FResources WaveEndResourcesGenerated;

	UFUNCTION()
	void GenerateWaveEndResources();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* BuildingMesh;
};
