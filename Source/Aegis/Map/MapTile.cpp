// Fill out your copyright notice in the Description page of Project Settings.


#include "MapTile.h"

#include "IContentBrowserSingleton.h"
#include "Aegis/AegisGameStateBase.h"
#include "Aegis/Enemies/Enemy.h"
#include "MapTiles/MapTileData.h"

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

void AMapTile::SetMapTileData(UMapTileData* Data)
{
	MapTileData = Data;

	const FVector NewMeshLocation = FVector(0, 0, MapTileData->Elevation * 40);
	TileMesh->SetRelativeLocation(NewMeshLocation);

	if (!MapTileData->bIsPath)
	{
		switch (MapTileData->TerrainType)
		{
		case Water:
			TileMesh->SetStaticMesh(WaterMesh);
			break;
		case Grass:
			TileMesh->SetStaticMesh(GrassMesh);
			break;
		case Cliff:
			TileMesh->SetStaticMesh(CliffMesh);
			break;
		default:
			TileMesh->SetStaticMesh(DefaultMesh);
			break;
		}
	} else
	{
		TileMesh->SetStaticMesh(PathMesh180);
	}
}

// Called when the game starts or when spawned
void AMapTile::BeginPlay()
{
	Super::BeginPlay();

	MapReference = Cast<AAegisGameStateBase>(GetWorld()->GetGameState())->AegisMap;

	CollisionVolume->OnComponentBeginOverlap.AddUniqueDynamic(this, &AMapTile::OnComponentBeginOverlap);
	CollisionVolume->OnComponentEndOverlap.AddUniqueDynamic(this, &AMapTile::OnComponentEndOverlap);
}

void AMapTile::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                       int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AEnemy* Enemy = Cast<AEnemy>(OtherActor))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Enemy on Tile"))
		EnemiesOnTile.Add(Enemy);
	}
}

void AMapTile::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                     int32 OtherBodyIndex)
{
	if (AEnemy* Enemy = Cast<AEnemy>(OtherActor))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Enemy off Tile"))
		EnemiesOnTile.Remove(Enemy);
	}
}
