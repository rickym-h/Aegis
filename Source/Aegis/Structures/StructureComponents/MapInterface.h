// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Map/MapTiles/MapTileData.h"
#include "Components/ActorComponent.h"
#include "MapInterface.generated.h"


class UAegisGameMap;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AEGIS_API UMapInterface : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UMapInterface();

	UFUNCTION(BlueprintCallable)
	int32 CountResourcesInRange(const EResourceType ResourceType, const int32 RangeInTiles, const bool IncludeSelfTile);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAegisGameMap* Map = nullptr;

	TArray<FTileCoord> OwnerCoords;

	UFUNCTION()
	void SetInternalData();
};
