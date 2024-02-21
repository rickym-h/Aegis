// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPawn.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Aegis/AegisGameStateBase.h"
#include "Aegis/Map/MapTile.h"
#include "Aegis/Structures/Towers/TowerData.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
#include "Input/InputConfigData.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APlayerPawn::APlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FocusPoint=CreateDefaultSubobject<UStaticMeshComponent>("Focus Point");
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
}

FHitResult* APlayerPawn::UpdateHitResultUnderCursor()
{
	const APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (!PlayerController) { return nullptr; }

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel3));
	PlayerController->GetHitResultUnderCursorForObjects(ObjectTypes, false, HitResultUnderCursor);
	return &HitResultUnderCursor;
}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerActionState == EPlayerState::Placing)
	{
		// Set the hologram static mesh to where the player is looking;
		UpdateHitResultUnderCursor();

		if (const AMapTile* MapTile = Cast<AMapTile>(HitResultUnderCursor.GetActor()))
		{
			StructureHologram->SetWorldLocation(MapTile->TileCoord.ToWorldLocation());
		}
	}
}

void APlayerPawn::Click(const FInputActionValue& InputActionValue)
{
	UpdateHitResultUnderCursor();

	const AMapTile* Tile = Cast<AMapTile>(HitResultUnderCursor.GetActor());
	if (!Tile) { return; }


	// Try to place tower
	if (PlayerActionState == EPlayerState::Placing)
	{
		UE_LOG(LogTemp, Warning, TEXT("ATTEMPTING TO PLACE STRUCTURE AT %ls"), *Tile->TileCoord.ToString())
		if (UTowerData* TowerData = Cast<UTowerData>(StructureToPlace))
		{
			if (GameState->AegisMap->AddStructureToMap(Tile->TileCoord, TowerData))
			{
				PlayerActionState = EPlayerState::Default;StructureToPlace = nullptr;
				StructureHologram->SetVisibility(false);
				RemoveTowerCardFromHand(TowerData);
			} else
			{
				UE_LOG(LogTemp, Error, TEXT("Could not place tower"))
			}
		} else
		{
			UE_LOG(LogTemp, Error, TEXT("Casting to UTowerData failed"))
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Click TileCoord: %ls"), *Tile->TileCoord.ToString())

	//DrawDebugSphere(GetWorld(), HitResultUnderCursor.Location, 5, 12, FColor::Red, false, 2, 0, 1);
}

void APlayerPawn::Move(const FInputActionValue& InputActionValue)
{
	//UE_LOG(LogTemp, Warning, TEXT("%ls") , *InputActionValue.ToString())
	AddMovementInput(FVector(InputActionValue.Get<FVector2D>().X, InputActionValue.Get<FVector2D>().Y, 0), 5);
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

bool APlayerPawn::AddTowerCardToHand(UTowerData* TowerData)
{
	TowerCardsInHand.Add(TowerData);
	OnTowersInHandUpdatedDelegate.Broadcast();
	return true;
}

bool APlayerPawn::RemoveTowerCardFromHand(UTowerData* TowerData)
{
	TowerCardsInHand.RemoveSingle(TowerData);
	OnTowersInHandUpdatedDelegate.Broadcast();
	return true;
}


void APlayerPawn::BeginPlacingStructure(UStructureData* StructureData)
{
	PlayerActionState = EPlayerState::Placing;
	StructureToPlace = StructureData;

	StructureHologram->SetVisibility(true);
}

