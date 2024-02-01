// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Map/AegisMap.h"
#include "GameFramework/Actor.h"
#include "BaseTower.generated.h"

UCLASS()
class AEGIS_API ABaseTower : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseTower();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* TowerMesh;

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Location Data")
	FTileCoord CurrentLocation;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
