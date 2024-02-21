// Fill out your copyright notice in the Description page of Project Settings.


#include "DefenderRangeComponent.h"

#include "Aegis/AegisGameStateBase.h"
#include "Aegis/Map/AegisMap.h"
#include "Aegis/Map/MapTile.h"
#include "Aegis/Enemies/Enemy.h"

// Sets default values for this component's properties
UDefenderRangeComponent::UDefenderRangeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UDefenderRangeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UDefenderRangeComponent::OnEnemyEnterRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AEnemy* Enemy = Cast<AEnemy>(OtherActor);
	if (!Enemy) { return; }

	TSet<AEnemy*> EnemiesInRange = GetAllEnemiesInRange();
	EnemiesInRange.Add(Enemy);

	OnEnemyEnterRangeDelegate.Broadcast(GetFrontEnemy(EnemiesInRange));
}

void UDefenderRangeComponent::InitRange(const FTileCoord DefenderCoord, const int Range)
{
	UE_LOG(LogTemp, Warning, TEXT("InitRange Setting to : %i" ), Range)
	const AAegisGameStateBase* GameState = Cast<AAegisGameStateBase>(GetWorld()->GetGameState());
	if (!GameState) { return; }
	
	for (const FTileCoord TileInRange : FTileCoord::GetTilesInRadius(DefenderCoord, Range))
	{
		// Get Tile
		AMapTile* Tile = GameState->AegisMap->GetTile(TileInRange);

		// Add tile to our TilesInRange TArray
		TilesInRange.Add(Tile);
		
		// Subscribe Enemy Entered range function to Overlap delegate in Tile Collision Mesh
		Tile->CollisionVolume->OnComponentBeginOverlap.AddUniqueDynamic(this, &UDefenderRangeComponent::OnEnemyEnterRange);
	}
}

TSet<AEnemy*> UDefenderRangeComponent::GetAllEnemiesInRange()
{
	TSet<AEnemy*> Enemies;
	for (const AMapTile* Tile : TilesInRange)
	{
		Enemies.Append(Tile->EnemiesOnTile);
	}
	return Enemies;
}

AEnemy* UDefenderRangeComponent::GetFrontEnemy(TSet<AEnemy*> Enemies)
{
	if (Enemies.Num() <= 0) { return nullptr; }

	// TODO find the front enemy to return rather than the first one
	for (AEnemy* Enemy : Enemies)
	{
		return Enemy;
	}
	return nullptr;
}
