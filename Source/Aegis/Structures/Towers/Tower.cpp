// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower.h"

#include "Components/DecalComponent.h"

// Sets default values
ATower::ATower()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SourcePoint = CreateDefaultSubobject<USceneComponent>("Source Point");
	SourcePoint->SetupAttachment(MeshOrigin);

	RangeIndicatorDecal = CreateDefaultSubobject<UDecalComponent>("Range Indicator Decal");
	RangeIndicatorDecal->SetupAttachment(SourcePoint);
	RangeIndicatorDecal->SetVisibility(false);
	RangeIndicatorDecal->SetRelativeLocation(FVector::ZeroVector);
	RangeIndicatorDecal->SetRelativeRotation(FRotator(90, 0, 0));
	RangeIndicatorDecal->DecalSize = FVector(500, 86.60254038, 86.60254038);
	RangeIndicatorDecal->SetWorldScale3D(FVector(1,1,1));
	
	//UMaterial* DecalMat = ConstructorHelpers::FObjectFinder<UMaterial>(TEXT("/Script/Engine.Material'/Game/Aegis/Art/Decals/M_RangeDecal_Rotating.M_RangeDecal_Rotating'")).Object;
	UMaterial* DecalMat = ConstructorHelpers::FObjectFinder<UMaterial>(TEXT("/Script/Engine.Material'/Game/Aegis/Art/Decals/M_RangeDecal_Static.M_RangeDecal_Static'")).Object;
	RangeIndicatorDecal->SetMaterial(0, DecalMat);

}

// Called when the game starts or when spawned
void ATower::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
