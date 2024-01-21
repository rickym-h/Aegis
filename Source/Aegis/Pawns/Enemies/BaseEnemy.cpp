// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemy.h"

#include "LocalizationConfigurationScript.h"
#include "Aegis/AegisGameStateBase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABaseEnemy::ABaseEnemy()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();

	GameState = Cast<AAegisGameStateBase>(GetWorld()->GetGameState());
	if (!GameState) { return; }
	
	FromTile = GameState->AegisMap->GetEnemySpawnCoord();
	GoalTile = GameState->AegisMap->GetNextCoordInPath(FromTile);
	UE_LOG(LogTemp, Warning, TEXT("FromTile = %ls"), *FromTile.ToString())
	UE_LOG(LogTemp, Warning, TEXT("GoalTile = %ls"), *GoalTile.ToString())

	SetActorLocation(FromTile.ToWorldLocation());
	
}

float ABaseEnemy::DistanceToGoalTile() const
{
	return FVector::Dist2D(GetActorLocation(), GoalTile.ToWorldLocation());
}

float ABaseEnemy::DistanceToGoalTile(const FVector& StartPos) const
{
	return FVector::Dist2D(StartPos, GoalTile.ToWorldLocation());
}

void ABaseEnemy::MoveTowardsGoal(float DeltaTime)
{
	// Get movement distance
	float DistanceToTravel = MovementSpeed * DeltaTime;
	//UE_LOG(LogTemp, Warning, TEXT("DistanceToTravel: %f"), DistanceToTravel)

	FVector TargetPos = GetActorLocation();
	// Compare with distance to goal
	while (DistanceToTravel > 0)
	{
		// If less than goal distance, just set target location towards goal
		if (DistanceToTravel <= DistanceToGoalTile(TargetPos))
		{
			FVector ForwardVector = GoalTile.ToWorldLocation() - FromTile.ToWorldLocation();
			ForwardVector.Normalize();
			TargetPos += ForwardVector * DistanceToTravel;
			DistanceToTravel = 0;
		} else
		// Otherwise take distance off movement distance, set new goal tiles, and recurse
		{
			DistanceToTravel -= DistanceToGoalTile(TargetPos);
			TargetPos = GoalTile.ToWorldLocation();
			FromTile = GoalTile;
			GoalTile = GameState->AegisMap->GetNextCoordInPath(FromTile);
			if (GoalTile == FromTile)
			{
				DistanceToTravel = 0;
			}
		}
	}
	SetActorLocation(TargetPos);
}

// Called every frame
void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveTowardsGoal(DeltaTime);
}
