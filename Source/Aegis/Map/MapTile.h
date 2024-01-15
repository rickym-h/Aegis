// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AegisMap.h"
#include "GameFramework/Actor.h"
#include "MapTile.generated.h"

UCLASS()
class AEGIS_API AMapTile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMapTile();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TileData")
	FTileCoord TileCoord;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TileData")
	float PathingGradient;

	UFUNCTION(BlueprintImplementableEvent)
	void ToggleShowGradients();

	UPROPERTY(BlueprintReadWrite)
	bool IsShowingGradients = false;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


};
