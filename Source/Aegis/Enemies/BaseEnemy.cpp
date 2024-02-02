// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemy.h"

#include "InputBehavior.h"
#include "LocalizationConfigurationScript.h"
#include "Aegis/AegisGameStateBase.h"
#include "Aegis/Map/AegisMap.h"
#include "Aegis/Buildings/NexusBuilding.h"
#include "Chaos/GeometryParticlesfwd.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABaseEnemy::ABaseEnemy()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>("Collision Sphere");
	RootComponent = CollisionSphere;
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(RootComponent);
	
	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);

	CollisionSphere->SetCollisionObjectType(ECC_GameTraceChannel2);
	CollisionSphere->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
	CollisionSphere->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Overlap);
}

// Called when the game starts or when spawned
void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();

	GameState = Cast<AAegisGameStateBase>(GetWorld()->GetGameState());
	if (!GameState) { return; }

	// Initialise pathing info
	FromTile = GameState->AegisMap->GetEnemySpawnCoord();
	GoalTile = GameState->AegisMap->GetNextCoordInPath(FromTile);

	// Set start location
	SetActorLocation(FromTile.ToWorldLocation());

	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ABaseEnemy::OverlapBegin);
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
	SetActorLocation(TargetPos, true);
}

void ABaseEnemy::OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Warning, TEXT("OVERLAP DETECTED"))

	if (Cast<ANexusBuilding>(OtherActor))
	{
		UGameplayStatics::ApplyDamage(OtherActor, DamageToNexus, GetWorld()->GetFirstPlayerController(), this, UDamageType::StaticClass());
		Destroy();
	}
	
}

// Called every frame
void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveTowardsGoal(DeltaTime);
}
