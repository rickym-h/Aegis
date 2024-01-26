// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseProjectileTower.h"

ABaseProjectileTower::ABaseProjectileTower()
{
	RangeOverlapMesh = CreateDefaultSubobject<UStaticMeshComponent>("Range Overlap Volume");
	RangeOverlapMesh->SetupAttachment(RootComponent);
}

void ABaseProjectileTower::BeginPlay()
{
	Super::BeginPlay();

}
