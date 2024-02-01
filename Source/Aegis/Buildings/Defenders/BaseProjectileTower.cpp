// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseProjectileTower.h"

#include "DefenderRangeComponent.h"
#include "Aegis/Pawns/Enemies/BaseEnemy.h"

ABaseProjectileTower::ABaseProjectileTower()
{
	RangeComponent = CreateDefaultSubobject<UDefenderRangeComponent>("Range Component");
}

void ABaseProjectileTower::BeginPlay()
{
	Super::BeginPlay();
	RangeComponent->InitRange(CurrentLocation, TowerRange);

	RangeComponent->OnEnemyEnterRangeDelegate.BindDynamic(this, &ABaseProjectileTower::OnEnemyEnterRange);
}

void ABaseProjectileTower::OnEnemyEnterRange(ABaseEnemy* OtherEnemy)
{
	UE_LOG(LogTemp, Warning, TEXT("Attempting shot...: %ls" ), *OtherEnemy->GetName())
}
