// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Map/MapTiles/MapTileData.h"
#include "Aegis/Pawns/ResourcesData.h"
#include "Aegis/Structures/Structure.h"
#include "ResourceCollector.generated.h"
class UMapInterface;

UCLASS()
class AEGIS_API AResourceCollector : public AStructure
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AResourceCollector();

	UFUNCTION()
	void InitResourceCollector(const FResources& InWaveEndResourcesGenerated, const TMap<TEnumAsByte<EResourceType>, FResources>& InResourcesToGeneratePerAdjacentResource);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resources")
	FResources WaveEndResourcesGenerated;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resources")
	TMap<TEnumAsByte<EResourceType>, FResources> ResourcesToGeneratePerAdjacentResource;

	UFUNCTION()
	void GenerateWaveEndResources();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* BuildingMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	UMapInterface* MapInterface;
};
