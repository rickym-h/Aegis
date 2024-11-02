// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AegisMap.h"
#include "GameFramework/Actor.h"
#include "MapTile.generated.h"

class AEnemy;

UCLASS()
class AEGIS_API AMapTile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMapTile();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tile Data")
	FTileCoord TileCoord;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemies on Tile")
	TSet<AEnemy*> EnemiesOnTile;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Tile Data")
	int TilesToEnd = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Tile Data")
	UMapTileData* MapTileData;

	void SetMapTileData(UMapTileData* Data);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Tile Data")
	FVector StructureLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* CollisionVolume;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Map")
	UAegisMap* MapReference;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Meshes")
	UStaticMesh* WaterMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Meshes")
	UStaticMesh* GrassMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Meshes")
	UStaticMesh* CliffMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Meshes")
	UStaticMesh* DefaultMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Meshes")
	UStaticMesh* BedrockMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Meshes")
	UStaticMesh* PathMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Meshes")
	UStaticMesh* TreesMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Meshes")
	UStaticMesh* StoneMesh;

	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	                             bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tile Data")
	float PathingGradient;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* TileMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* ResourceMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	TArray<UStaticMeshComponent*> BedrockMeshes;
};
