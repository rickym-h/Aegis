// Fill out your copyright notice in the Description page of Project Settings.


#include "Structure.h"

// Sets default values
AStructure::AStructure()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshOrigin = CreateDefaultSubobject<USceneComponent>("Mesh Origin");
	SetRootComponent(MeshOrigin);
}

// Called when the game starts or when spawned
void AStructure::BeginPlay()
{
	Super::BeginPlay();
}
