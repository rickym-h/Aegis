// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Map/AegisMap.h"
#include "Components/ActorComponent.h"
#include "DefenderRangeComponent.generated.h"


class AEnemy;
class AMapTile;
// Delegate signature
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyEnterRangeSignature, AEnemy*, OutEnemy);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AEGIS_API UDefenderRangeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDefenderRangeComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	TArray<AMapTile*> TilesInRange;

	UFUNCTION()
	void OnEnemyEnterRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	UPROPERTY()
	FOnEnemyEnterRangeSignature OnEnemyEnterRangeDelegate;
	
	void InitRange(const FTileCoord DefenderCoord, const int Range);

	TSet<AEnemy*> GetAllEnemiesInRange();

	static AEnemy* GetFrontEnemy(const TSet<AEnemy*>& Enemies);
};
