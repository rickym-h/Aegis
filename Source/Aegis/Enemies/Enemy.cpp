// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "Aegis/Core/GameStates/AegisGameStateBase.h"
#include "Aegis/Map/AegisGameMap.h"
#include "Aegis/Map/MapTile.h"
#include "Aegis/Structures/NexusBuilding/NexusBuilding.h"
#include "Components/CapsuleComponent.h"
#include "Components/StatusEffectComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemy::AEnemy()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root Scene Component");
	
	CollisionCapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>("Collision Capsule");
	CollisionCapsuleComponent->SetupAttachment(RootComponent);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(RootComponent);

	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionCapsuleComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionCapsuleComponent->SetCollisionObjectType(ECC_GameTraceChannel2);
	CollisionCapsuleComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
	CollisionCapsuleComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Overlap);
	CollisionCapsuleComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECR_Overlap);

	TargetPoint = CreateDefaultSubobject<USceneComponent>("Target Point");
	TargetPoint->SetupAttachment(RootComponent);

	StatusEffectComponent = CreateDefaultSubobject<UStatusEffectComponent>("StatusEffect");
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;

	GameState = Cast<AAegisGameStateBase>(GetWorld()->GetGameState());
	if (!GameState)
	{
		UE_LOG(LogTemp, Error, TEXT("AEnemy::BeginPlay() - No GameState reference found."))
		Destroy();
		return;
	}

	if (!GameState->AegisMap)
	{
		UE_LOG(LogTemp, Error, TEXT("AEnemy::BeginPlay() - No AegisMap reference found."))
		Destroy();
		return;
	}

	// Initialise pathing info
	FromTile = GameState->AegisMap->GetEnemySpawnCoord();
	GoalTile = GameState->AegisMap->GetNextCoordInPath(FromTile);

	// Set start location
	SetActorLocation(FromTile.ToWorldLocation());

	CollisionCapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OverlapBegin);
	
	CurrentMovementSpeed = MovementSpeed;

	StatusEffectComponent->OnSpeedMultiplierChangedDelegate.AddUniqueDynamic(this, &AEnemy::OnSpeedMultiplierChanged);
}

UStatusEffectComponent* AEnemy::GetStatusEffectComponent() const
{
	return StatusEffectComponent;
}

void AEnemy::OnSpeedMultiplierChanged(const float NewSpeedMultiplier)
{
	CurrentMovementSpeed = NewSpeedMultiplier * MovementSpeed;
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Health -= DamageAmount;
	if (Health <= 0)
	{
		Destroy();
	}
	return DamageAmount;
}

float AEnemy::DistanceToGoalTile() const
{
	return FVector::Dist2D(GetActorLocation(), GoalTile.ToWorldLocation());
}

float AEnemy::DistanceToGoalTile(const FVector& StartPos) const
{
	return FVector::Dist2D(StartPos, GoalTile.ToWorldLocation());
}

void AEnemy::MoveTowardsGoal(float DeltaTime)
{
	// Get movement distance
	float DistanceToTravel = CurrentMovementSpeed * DeltaTime;
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
			SetActorRotation(ForwardVector.Rotation());
		}
		else
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

void AEnemy::OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
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
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveTowardsGoal(DeltaTime);
}

float AEnemy::GetDistanceToNexus() const
{
	float DistanceToNexus = ((GameState->AegisMap->GetTile(GoalTile)->TilesToEnd) * 100 * FMath::Sqrt(3.f));
	DistanceToNexus += DistanceToGoalTile();

	return DistanceToNexus;
}

float AEnemy::GetHealthAsPercentage() const
{
	//const float Percentage = FMath::Clamp(0, 1, Health / MaxHealth);
	//UE_LOG(LogTemp, Warning, TEXT("%f"), Percentage)
	return Health / MaxHealth;
}
