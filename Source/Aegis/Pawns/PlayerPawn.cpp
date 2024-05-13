// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPawn.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ResourcesData.h"
#include "Aegis/AegisGameStateBase.h"
#include "Aegis/Map/MapTile.h"
#include "Aegis/Structures/StructureDataFactory.h"
#include "Aegis/Structures/Towers/TowerData.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
#include "Input/InputConfigData.h"

// Sets default values
APlayerPawn::APlayerPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FocusPoint = CreateDefaultSubobject<UStaticMeshComponent>("Focus Point");
	RootComponent = FocusPoint;
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("Spring Arm");
	SpringArm->SetupAttachment(RootComponent);
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm);

	StructureHologram = CreateDefaultSubobject<UStaticMeshComponent>("Hologram Structure");
	StructureHologram->SetupAttachment(RootComponent);
	StructureHologram->SetVisibility(false);
	StructureHologram->SetCollisionResponseToAllChannels(ECR_Ignore);

	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>("Floating Movement Component");

	Resources = CreateDefaultSubobject<UResourcesData>("Resources");

	PlayerActionState = Default;
}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	if (Cast<AAegisGameStateBase>(GetWorld()->GetGameState()))
	{
		GameState = Cast<AAegisGameStateBase>(GetWorld()->GetGameState());
	}

	StructureHologram->SetWorldLocation(FTileCoord().ToWorldLocation());

	if (GameState)
	{
		TowerCardsInHand.Append(GameState->StructureDataFactory->GenerateStarterTowers(GetWorld()));
	}

	UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::BeginPlay()"))
	OnTowersInHandUpdatedDelegate.Broadcast();
}

FHitResult* APlayerPawn::UpdateHitResultUnderCursor()
{
	const APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (!PlayerController) { return nullptr; }

	// TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	// ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel3));
	// PlayerController->GetHitResultUnderCursorForObjects(ObjectTypes, false, HitResultUnderCursor);

	PlayerController->GetHitResultUnderCursor(ECC_Visibility, true, HitResultUnderCursor);

	return &HitResultUnderCursor;
}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerActionState == Placing)
	{
		// Set the hologram static mesh to where the player is looking;
		UpdateHitResultUnderCursor();

		if (const AMapTile* MapTile = Cast<AMapTile>(HitResultUnderCursor.GetActor()))
		{
			StructureHologram->SetWorldLocation(MapTile->StructureLocation);

			// Update StructureHologram
			UpdateStructureHologramMesh(StructureToPlace->CanStructureBePlaced(MapTile->TileCoord));
		}
	}

	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, FMath::Clamp(BoomArmTargetLength, 300, 10000),
	                                              GetWorld()->DeltaRealTimeSeconds, 10);
}

void APlayerPawn::Click(const FInputActionValue& InputActionValue)
{
	UpdateHitResultUnderCursor();

	const FVector Location = HitResultUnderCursor.Location;
	//DrawDebugSphere(GetWorld(), Location, 200, 13, FColor::Green, false, 3, 0, 3);
	UE_LOG(LogTemp, Warning, TEXT("Round the FVector: %ls"), *Location.ToString())
	//const FTileCoord RoundedCoord = FTileCoord::PixelToHex(Location);
	//DrawDebugSphere(GetWorld(), RoundedCoord.ToWorldLocation(), 200, 12, FColor::Red, false, 3, 0, 3);


	const AMapTile* Tile = Cast<AMapTile>(HitResultUnderCursor.GetActor());
	if (!Tile) { return; }

	// Try to place tower if a tower is selected
	if (PlayerActionState == Placing)
	{
		if (UStructureData* StructureData = Cast<UStructureData>(StructureToPlace))
		{
			if (GameState->AegisMap->AddStructureToMap(Tile->TileCoord, StructureData))
			{
				PlayerActionState = Default;
				StructureToPlace = nullptr;
				StructureHologram->SetVisibility(false);
				if (StructureData->bRemoveInstanceOnPlacement)
				{
					RemoveTowerCardFromHand(StructureData);
				}
			}
		}
	}
}

void APlayerPawn::Move(const FInputActionValue& InputActionValue)
{
	// Get the target zoom location
	BoomArmTargetLength += (InputActionValue.Get<FVector>().Z * 1000);
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
	APlayerController* PC = Cast<APlayerController>(GetController());

	// Get the local player subsystem
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	// Clear out existing mapping, and add our mapping
	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(InputMapping, 0);

	// Get the EnhancedInputComponent
	UEnhancedInputComponent* PEI = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	// Bind the actions
	PEI->BindAction(InputActions->InputClick, ETriggerEvent::Triggered, this, &APlayerPawn::Click);
	PEI->BindAction(InputActions->InputMove, ETriggerEvent::Triggered, this, &APlayerPawn::Move);
}

bool APlayerPawn::AddTowerCardToHand(UStructureData* TowerData)
{
	TowerCardsInHand.Add(TowerData);
	OnTowersInHandUpdatedDelegate.Broadcast();
	return true;
}

bool APlayerPawn::RemoveTowerCardFromHand(UStructureData* TowerData)
{
	TowerCardsInHand.RemoveSingle(TowerData);
	OnTowersInHandUpdatedDelegate.Broadcast();
	return true;
}


void APlayerPawn::BeginPlacingStructure(UStructureData* StructureData)
{
	PlayerActionState = Placing;
	StructureToPlace = StructureData;

	// if (UStaticMesh* Mesh = StructureToPlace->GetMeshRepresentation())
	// {
	// 	StructureHologram->SetStaticMesh(Mesh);
	// }

	StructureHologram->SetVisibility(true);
}
