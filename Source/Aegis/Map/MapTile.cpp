// Fill out your copyright notice in the Description page of Project Settings.


#include "MapTile.h"

#include "AegisGameMap.h"
#include "Aegis/Core/GameStates/AegisGameStateBase.h"
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

	ResourceMesh = CreateDefaultSubobject<UStaticMeshComponent>("Resource Mesh");
	ResourceMesh->SetupAttachment(RootComponent);
	ResourceMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	ResourceMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void AMapTile::SetMapTileData(UMapTileData* Data)
{
	MapTileData = Data;
	
	// Create bedrock meshes (if any)
	for (int i = 0; i <= MapTileData->Elevation; i++)
	{
		// Create a bedrock static mesh component
		const FName ComponentName = FName(FString("BedRockLayer") + FString::FromInt(i));
		UStaticMeshComponent* BedrockStaticMeshComp = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass(), ComponentName);

		BedrockStaticMeshComp->SetupAttachment(this->RootComponent);

		BedrockStaticMeshComp->SetRelativeLocationAndRotation(FVector(0,0,-50) + (FVector(0,0,50) * i), FRotator::ZeroRotator);
		BedrockStaticMeshComp->SetStaticMesh(BedrockMesh);

		BedrockStaticMeshComp->RegisterComponent();
		BedrockStaticMeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);

		BedrockMeshes.Push(BedrockStaticMeshComp);
	}

	// Set the top tile SM and location based on ground type
	const FVector TileMeshLocation = (MapTileData->Elevation) * FVector(0, 0, 50);
	TileMesh->SetRelativeLocation(TileMeshLocation);

	if (!MapTileData->bIsPath)
	{
		switch (MapTileData->TerrainType)
		{
		case Water:
			TileMesh->SetStaticMesh(WaterMesh);
			TileMesh->SetRelativeScale3D(FVector(1, 1, 0.6));
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
	}
	else
	{
		TileMesh->SetStaticMesh(PathMesh);
		TileMesh->SetRelativeScale3D(FVector(1, 1, 0.75));
	}

	StructureLocation = TileCoord.ToWorldLocation() + TileMeshLocation + FVector(0, 0, 50);

	// Set the resource mesh SM and location
	ResourceMesh->SetWorldLocation(StructureLocation);

	switch (MapTileData->ResourceType)
	{
	case Tree:
		ResourceMesh->SetStaticMesh(TreesMesh);
		break;
	case Stone:
		ResourceMesh->SetStaticMesh(StoneMesh);
		break;
	default:
		ResourceMesh->SetStaticMesh(nullptr);
		break;
	}
}

// Called when the game starts or when spawned
void AMapTile::BeginPlay()
{
	Super::BeginPlay();

	MapReference = Cast<AAegisGameStateBase>(GetWorld()->GetGameState())->AegisMap;

	CollisionVolume->OnComponentBeginOverlap.AddUniqueDynamic(this, &AMapTile::OnComponentBeginOverlap);
	CollisionVolume->OnComponentEndOverlap.AddUniqueDynamic(this, &AMapTile::OnComponentEndOverlap);
	
	ResourceMesh->SetRelativeRotation(FRotator(0, FMath::RandRange(0, 360),0));
}

void AMapTile::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                       int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AEnemy* Enemy = Cast<AEnemy>(OtherActor))
	{
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
