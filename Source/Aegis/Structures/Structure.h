// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Map/AegisMap.h"
#include "GameFramework/Actor.h"
#include "Structure.generated.h"


class UStructureCard;

UCLASS()
class AEGIS_API AStructure : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AStructure();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Structure Data", meta=(ToolTip="Origin point for the mesh, so any visible meshes part of this structure should be attached to this"))
	USceneComponent* MeshOrigin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Structure Data")
	FTileCoord CurrentLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Structure Data")
	UStructureCard* StructureCard;
	
	FTransform ActorTransform;
};
