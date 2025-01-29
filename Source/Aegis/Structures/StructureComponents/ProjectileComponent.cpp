// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileComponent.h"

#include "Aegis/Core/GameStates/AegisGameStateBase.h"
#include "Aegis/Structures/Structure.h"
#include "Aegis/Utilities/ProjectileManager.h"

// Sets default values for this component's properties
UProjectileComponent::UProjectileComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UProjectileComponent::BeginPlay()
{
	Super::BeginPlay();

	GameState = Cast<AAegisGameStateBase>(GetWorld()->GetGameState());
	if (!GameState)
	{
		UE_LOG(LogTemp, Error, TEXT("UProjectileComponent::BeginPlay - GameState not found!!"))
	}
}

void UProjectileComponent::FireArrow(const FVector& StartPoint, const AEnemy* Enemy, const float Damage) const
{
	if (!GameState)
	{
		UE_LOG(LogTemp, Error, TEXT("UProjectileComponent::FireArrowAtEnemy - GameState not found!!"))
		return;
	}
	
	FProjectileDamagePackage DamagePackage = FProjectileDamagePackage();
	DamagePackage.PhysicalDamage = Damage;

	UStaticMesh* ArrowMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Script/Engine.StaticMesh'/Game/Aegis/Art/VoxelArt/SM_Arrow.SM_Arrow'"));

	const FVector EndControlPoint = Enemy->TargetPoint->GetComponentLocation();
	FVector CentreControlPoint = (StartPoint + EndControlPoint)/2;
	CentreControlPoint.Z = StartPoint.Z + 50 + ((FVector::DistXY(StartPoint, EndControlPoint)/10.f));
	
	float ApproximateBezierLength = 0;
	FVector PreviousBezierPoint = StartPoint;
	for (int i = 1; i < 25; i++)
	{
		float T = 1.f/static_cast<float>(i);
		const FVector StartToCentrePoint = FMath::Lerp(StartPoint, CentreControlPoint, T);
		const FVector CentreToEndPoint = FMath::Lerp(CentreControlPoint, EndControlPoint, T);
		const FVector BezierPoint = FMath::Lerp(StartToCentrePoint, CentreToEndPoint, T);

		ApproximateBezierLength += FVector::Dist(PreviousBezierPoint, BezierPoint);

		PreviousBezierPoint = BezierPoint;
	}
	ApproximateBezierLength /= 100; // Convert bezier length in cm to m.

	constexpr float ProjectileSpeed = 20.f;
	const float StartToEndTime = ApproximateBezierLength / ProjectileSpeed;
	
	GameState->ProjectileManager->LaunchArcProjectile(
		StartPoint,
		CentreControlPoint,
		EndControlPoint,
		ArrowMesh,
		StartToEndTime,
		DamagePackage,
		GetOwner(),
		nullptr, nullptr
	);
}