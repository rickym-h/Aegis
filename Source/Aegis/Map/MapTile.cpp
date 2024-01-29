// Fill out your copyright notice in the Description page of Project Settings.


#include "MapTile.h"

#include "Chaos/AABBTree.h"

// Sets default values
AMapTile::AMapTile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	CollisionVolume = CreateDefaultSubobject<UStaticMeshComponent>("Collision Volume");
	RootComponent = CollisionVolume;
	CollisionVolume->SetVisibility(false);
	CollisionVolume->SetCollisionObjectType(ECC_GameTraceChannel3);
	CollisionVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionVolume->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Overlap);

	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>("Tile Mesh");
	TileMesh->SetupAttachment(RootComponent);
	TileMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	TileMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

// Called when the game starts or when spawned
void AMapTile::BeginPlay()
{
	Super::BeginPlay();

}