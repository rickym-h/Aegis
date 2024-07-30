// Fill out your copyright notice in the Description page of Project Settings.


#include "DefenderRangeComponent.h"

#include "Aegis/Map/AegisMap.h"
#include "Aegis/Enemies/Enemy.h"

// Sets default values for this component's properties
UDefenderRangeComponent::UDefenderRangeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
	SetVisibility(false);
	UPrimitiveComponent::SetCollisionObjectType(ECC_GameTraceChannel3);
	UPrimitiveComponent::SetCollisionResponseToAllChannels(ECR_Ignore);
	UPrimitiveComponent::SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Overlap);
}


// Called when the game starts
void UDefenderRangeComponent::BeginPlay()
{
	Super::BeginPlay();
	
	SetCapsuleSize(86.60254038, 86.60254038+300);

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
	// Set the range capsule
	const int Scale = 1+(Range*2);
	SetWorldScale3D(FVector(1, Scale, Scale));
	OnComponentBeginOverlap.AddUniqueDynamic(this, &UDefenderRangeComponent::OnEnemyEnterRange);
	
	// Check if enemies are around as soon as the tower is spawned
	const TSet<AEnemy*> Enemies = GetAllEnemiesInRange();
	if (AEnemy* Enemy = GetFrontEnemy(Enemies))
	{
		OnEnemyEnterRangeDelegate.Broadcast(Enemy);
	}
	
}

TSet<AEnemy*> UDefenderRangeComponent::GetAllEnemiesInRange()
{
	TSet<AEnemy*> Enemies;

	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, AEnemy::StaticClass());

	for (AActor* Actor : OverlappingActors)
	{
		if (AEnemy* Enemy = Cast<AEnemy>(Actor))
		{
			Enemies.Add(Enemy);
		}
	}
	
	return Enemies;
}

AEnemy* UDefenderRangeComponent::GetFrontEnemy(const TSet<AEnemy*>& Enemies)
{
	if (Enemies.Num() <= 0) { return nullptr; }

	TArray<AEnemy*> EnemiesList = Enemies.Array();

	AEnemy* FrontEnemy = EnemiesList[0];
	float FrontEnemyDistanceToNexus = FrontEnemy->GetDistanceToNexus();

	for (AEnemy* Enemy : EnemiesList)
	{
		const float CurrentEnemyDistanceToNexus = Enemy->GetDistanceToNexus();
		if (CurrentEnemyDistanceToNexus < FrontEnemyDistanceToNexus)
		{
			FrontEnemy = Enemy;
			FrontEnemyDistanceToNexus = CurrentEnemyDistanceToNexus;
		}
	}
	return FrontEnemy;
}
