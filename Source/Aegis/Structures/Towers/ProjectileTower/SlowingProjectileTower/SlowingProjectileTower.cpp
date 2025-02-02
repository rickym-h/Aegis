// Fill out your copyright notice in the Description page of Project Settings.


#include "SlowingProjectileTower.h"

#include "Aegis/Enemies/Enemy.h"
#include "Aegis/Enemies/Components/StatusEffectComponent.h"


// Sets default values
ASlowingProjectileTower::ASlowingProjectileTower()
{

}

void ASlowingProjectileTower::InitSlowingProjectileTower(const EProjectileType InProjectileType, const FProjectileDamagePackage InProjectileDamagePackage, const float InAttackSpeed,
	const int32 InRangeTiles, UStaticMesh* InProjectileMesh, const FSlowEffect InEffectToApply)
{
	InitProjectileTower(InProjectileType, InProjectileDamagePackage, InProjectileMesh, InAttackSpeed, InRangeTiles);

	EffectToApply = InEffectToApply;
}

// Called when the game starts or when spawned
void ASlowingProjectileTower::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASlowingProjectileTower::SingleTargetProjectileCallback_Implementation(AEnemy* HitEnemy, const float DamageApplied, const FHitResult& HitResult)
{
	UE_LOG(LogTemp, Warning, TEXT("ASlowingProjectileTower::SingleTargetProjectileCallback_Implementation - Applied slow effect!"))
	HitEnemy->GetStatusEffectComponent()->ApplySlowEffect(EffectToApply);
}
