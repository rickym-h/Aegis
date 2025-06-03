// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileComponent.h"

#include "Aegis/Core/GameStates/AegisGameStateBase.h"
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

FVector UProjectileComponent::CalculateCentreControlPoint(const FVector& StartPoint, const AEnemy* Enemy)
{
	const FVector EndControlPoint = Enemy->TargetPoint->GetComponentLocation();
	return CalculateCentreControlPoint(StartPoint, EndControlPoint);
}

FVector UProjectileComponent::CalculateCentreControlPoint(const FVector& StartPoint, const FVector& EndPoint)
{
	FVector CentreControlPoint = (StartPoint + EndPoint)/2;
	CentreControlPoint.Z = StartPoint.Z + 30 + ((FVector::DistXY(StartPoint, EndPoint)/10.f));
	return CentreControlPoint;
}

float UProjectileComponent::ApproximateBezierLength(const FVector& StartPoint, const FVector CentreControlPoint, const FVector EndControlPoint)
{
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
	return ApproximateBezierLength;
}

void UProjectileComponent::FireArcArrow(const FVector& StartPoint, const AEnemy* Enemy, const float Damage) const
{
	if (!GameState)
	{
		UE_LOG(LogTemp, Error, TEXT("UProjectileComponent::FireArrowAtEnemy - GameState not found!!"))
		return;
	}
	
	FProjectileDamagePackage DamagePackage = FProjectileDamagePackage();
	DamagePackage.PhysicalDamage = Damage;

	UStaticMesh* ArrowMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Script/Engine.StaticMesh'/Game/Aegis/Art/VoxelArt/SM_Arrow.SM_Arrow'"));

	const FVector CentreControlPoint = CalculateCentreControlPoint(StartPoint, Enemy);
	const FVector EndControlPoint = Enemy->TargetPoint->GetComponentLocation();
	
	const float BezierLength = ApproximateBezierLength(StartPoint, CentreControlPoint, EndControlPoint);

	constexpr float ProjectileSpeed = 40.f;
	const float StartToEndTime = BezierLength / ProjectileSpeed;
	
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

void UProjectileComponent::FireRunicSpark(const FVector& StartPoint, const AEnemy* Enemy, const float Damage) const
{
	if (!GameState)
	{
		UE_LOG(LogTemp, Error, TEXT("UProjectileComponent::FireArrowAtEnemy - GameState not found!!"))
		return;
	}

	FProjectileDamagePackage DamagePackage = FProjectileDamagePackage();
	DamagePackage.MagicDamage = Damage;

	UStaticMesh* Sparkmesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Script/Engine.StaticMesh'/Game/Aegis/Art/VoxelArt/SM_Arrow.SM_Arrow'")); // TODO change to custom mesh
	
	GameState->ProjectileManager->LaunchHomingProjectile(
		StartPoint,
		Enemy,
		Sparkmesh,
		4,
		10,
		1,
		DamagePackage,
		GetOwner(),
		nullptr, nullptr
	);
}

void UProjectileComponent::FireSimpleArcProjectile(const FVector& StartPoint, const FVector& EndPoint, const FProjectileDamagePackage DamagePackage,
	float ProjectileSpeed, UStaticMesh* ProjectileMesh)
{
	if (!GameState)
	{
		UE_LOG(LogTemp, Error, TEXT("UProjectileComponent::FireArrowAtEnemy - GameState not found!!"))
		return;
	}
	const FVector CentreControlPoint = CalculateCentreControlPoint(StartPoint, EndPoint);
	const float BezierLength = ApproximateBezierLength(StartPoint, CentreControlPoint, EndPoint);
	const float StartToEndTime = BezierLength / ProjectileSpeed;
	
	GameState->ProjectileManager->LaunchArcProjectile(
		StartPoint,
		CentreControlPoint,
		EndPoint,
		ProjectileMesh,
		StartToEndTime,
		DamagePackage,
		GetOwner(),
		nullptr, nullptr
	);	
}

void UProjectileComponent::FireCustomArcProjectile(const FVector& StartPoint, const FVector& CentreControlPoint, const FVector& EndPoint,
                                                   const FProjectileDamagePackage DamagePackage, const float ProjectileSpeed, UStaticMesh* ProjectileMesh) const
{
	if (!GameState)
	{
		UE_LOG(LogTemp, Error, TEXT("UProjectileComponent::FireArrowAtEnemy - GameState not found!!"))
		return;
	}

	const float BezierLength = ApproximateBezierLength(StartPoint, CentreControlPoint, EndPoint);
	const float StartToEndTime = BezierLength / ProjectileSpeed;
	
	GameState->ProjectileManager->LaunchArcProjectile(
		StartPoint,
		CentreControlPoint,
		EndPoint,
		ProjectileMesh,
		StartToEndTime,
		DamagePackage,
		GetOwner(),
		nullptr, nullptr
	);
}
