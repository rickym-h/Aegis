// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TileRangeComponent.generated.h"

class AEnemy;
struct FTileCoord;
class AMapTile;
// Delegate signature
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyEnterTileRangeSignature, AEnemy*, OutEnemy);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AEGIS_API UTileRangeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTileRangeComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	TArray<AMapTile*> TilesInRange;

	UFUNCTION()
	void OnEnemyEnterRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
						   bool bFromSweep, const FHitResult& SweepResult);

public:
	UPROPERTY()
	FOnEnemyEnterTileRangeSignature OnEnemyEnterRangeDelegate;

	void InitRange(const FTileCoord OriginCoord, const int RangeInTiles);

	TSet<AEnemy*> GetAllEnemiesInRange();

	static AEnemy* GetFrontEnemy(const TSet<AEnemy*>& Enemies);
};
