// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPawn.h"

#include "AegisPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Aegis/AegisGameStateBase.h"
#include "Aegis/Cards/StructureCard.h"
#include "Aegis/Map/MapTile.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
#include "Input/InputConfigData.h"

// Sets default values
APlayerPawn::APlayerPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FocusPoint = CreateDefaultSubobject<USceneComponent>("Focus Point");
	RootComponent = FocusPoint;
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("Spring Arm");
	SpringArm->SetupAttachment(RootComponent);
	BoomArmTargetLength = 2000;
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm);

	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>("Floating Movement Component");

	RangeIndicatorDecal = CreateDefaultSubobject<UDecalComponent>("Range Indicator Decal");
	RangeIndicatorDecal->SetVisibility(false);
	RangeIndicatorDecal->SetWorldLocation(FVector::ZeroVector);
	RangeIndicatorDecal->SetWorldRotation(FRotator(90, 0, 0));
	RangeIndicatorDecal->DecalSize = FVector(500, 100, 100);
	RangeIndicatorDecal->SetWorldScale3D(FVector(1,1,1));
}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	GameState = Cast<AAegisGameStateBase>(GetWorld()->GetGameState());
	if (GameState)
	{
		
	} else
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerPawn::BeginPlay - Could not set GameState!"))
	}

	AegisController = Cast<AAegisPlayerController>(GetController());
	if (AegisController)
	{
		AegisController->OnSelectedCardUpdatedDelegate.AddUniqueDynamic(this, &APlayerPawn::UpdateSelectedCard);
	} else
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerPawn::BeginPlay - Could not find Controller!"))
	}
}

void APlayerPawn::ClearStructureHolograms()
{
	for(UStaticMeshComponent* Component : StructureHolograms)
	{
		Component->DestroyComponent();
	}
	StructureHolograms = TArray<UStaticMeshComponent*>();
}

void APlayerPawn::UpdateSelectedCard()
{
	SelectedCard = AegisController->GetSelectedCard();

	// Set holograms and decals, or clear if SelectedCard is not valid
	if (SelectedCard)
	{
		// Clear any possibly existing hologram components
		ClearStructureHolograms();

		// Create a new hologram component for each structure offset
		if (const UStructureCard* StructureCard = Cast<UStructureCard>(SelectedCard))
		{
			for (int i = 0; i < StructureCard->StructureOffsets.Num(); i++)
			{
				//UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::UpdateSelectedCard - Creating hologram component: %i!"), i)
				UStaticMeshComponent* StructureHologramComp = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass());
				StructureHologramComp->RegisterComponent();
				
				StructureHologramComp->SetVisibility(false);
				StructureHologramComp->SetCollisionResponseToAllChannels(ECR_Ignore);
				StructureHologramComp->SetWorldLocation(FTileCoord().ToWorldLocation());
				StructureHolograms.Add(StructureHologramComp);
				
				StructureHologramComp->SetVisibility(true);
			}
		}
	} else
	{
		ClearStructureHolograms();
	}
}

void APlayerPawn::UpdateHologramPositions()
{
	UStructureCard* StructureCard = Cast<UStructureCard>(SelectedCard);
	if (!StructureCard)
	{
		return;
	}
	
	if (!AegisController->GetHoveredHitResult()->IsValidBlockingHit())
	{
		return;
	}

	const FTileCoord LocationCoord = FTileCoord::FromWorldLocation(AegisController->GetHoveredHitResult()->Location);
	const AMapTile* CentreTile = GameState->AegisMap->GetTile(LocationCoord);
	
	if (!CentreTile)
	{
		UE_LOG(LogTemp, Error, TEXT("APlayerPawn::UpdateHologramPositions - No MapTile found for hovered location, aborting!"))
		return;
	}

	const bool bIsPlacementPossible = GameState->AegisMap->CanStructureBePlaced(StructureCard, LocationCoord);

	// Set Offset holograms for the offsets
	for (int i = 0; i < StructureCard->StructureOffsets.Num(); i++)
	{
		if (!StructureHolograms.IsValidIndex(i))
		{
			UE_LOG(LogTemp, Error, TEXT("APlayerPawn::UpdateHologramPositions - Structure hologram index [%i] not valid! Maybe not enough available hologram components?"), i)
			continue;
		}
		if (const AMapTile* OffsetTile = GameState->AegisMap->GetTile(LocationCoord + StructureCard->StructureOffsets[i]))
		{
			const FVector OffsetTileStructureLocation = OffsetTile->StructureLocation;
			StructureHolograms[i]->SetWorldLocation(OffsetTileStructureLocation);
		} else
		{
			UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::UpdateHologramPositions - No valid location for offset tile!"))
			FVector OffsetVector = (LocationCoord + StructureCard->StructureOffsets[i]).ToWorldLocation();
			OffsetVector.Z = CentreTile->StructureLocation.Z;
			StructureHolograms[i]->SetWorldLocation(OffsetVector);
		}
		UpdateStructureHologramMesh(StructureHolograms[i], bIsPlacementPossible);
		StructureHolograms[i]->SetVisibility(true);
	}
}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Handle camera movement
	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, FMath::Clamp(BoomArmTargetLength, 300, 10000), GetWorld()->DeltaRealTimeSeconds, 10);

	if (SelectedCard)
	{
		// If structure card, update hologram positions
		if (Cast<UStructureCard>(SelectedCard))
		{
			UpdateHologramPositions();
		}
		
		// TODO If card has range, update decal positions
	}
	
}

void APlayerPawn::Move(const FInputActionValue& InputActionValue)
{
	// Get the target zoom location
	BoomArmTargetLength += InputActionValue.Get<FVector>().Z * 1000;
	BoomArmTargetLength = FMath::Clamp(BoomArmTargetLength, 0, 10000);

	MovementComponent->MaxSpeed = 2000 + FMath::Pow(SpringArm->TargetArmLength, 0.95);
	MovementComponent->Acceleration = MovementComponent->MaxSpeed * 4;
	MovementComponent->Deceleration = MovementComponent->MaxSpeed * 9;

	AddMovementInput(FVector(InputActionValue.Get<FVector>().X, InputActionValue.Get<FVector>().Y, 0), FMath::Pow(SpringArm->TargetArmLength, 1.01));
}

// Called to bind functionality to input
void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Get the player controller
	const APlayerController* PC = Cast<APlayerController>(GetController());

	// Get the local player subsystem
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	// Clear out existing mapping, and add our mapping
	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(InputMapping, 0);

	// Get the EnhancedInputComponent
	UEnhancedInputComponent* PEI = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	// Bind the actions
	PEI->BindAction(InputActions->InputMove, ETriggerEvent::Triggered, this, &APlayerPawn::Move);
}
