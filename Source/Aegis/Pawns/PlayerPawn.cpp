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

	if (GameState)
	{
		TowerCardsInHand.Append(GameState->StructureDataFactory->GenerateStarterTowers(GetWorld()));
	}

	UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::BeginPlay()"))
	OnTowersInHandUpdatedDelegate.Broadcast();
}

void APlayerPawn::ClearStructureHolograms()
{
	for(UStaticMeshComponent* Component : StructureHolograms)
	{
		Component->DestroyComponent();
	}
	StructureHolograms = TArray<UStaticMeshComponent*>();
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
			StructureHolograms[0]->SetWorldLocation(MapTile->StructureLocation);
			bool bIsPlacementPossible = StructureToPlace->CanStructureBePlaced(MapTile->TileCoord);
			UpdateStructureHologramMesh(StructureHolograms[0], bIsPlacementPossible);


			for (int i = 0; i < StructureToPlace->StructureOffsets.Num(); i++)
			{
				const AMapTile* OffsetTile = GameState->AegisMap->GetTile(MapTile->TileCoord + StructureToPlace->StructureOffsets[i]);
				if (!OffsetTile)
				{
					StructureHolograms[1+i]->SetVisibility(false);
					continue;
				}
				
				const FVector StructureOffsetVector = OffsetTile->StructureLocation;
				 
				StructureHolograms[1+i]->SetWorldLocation(StructureOffsetVector);
				StructureHolograms[1+i]->SetVisibility(true);
				
				// Update StructureHologram.
				UpdateStructureHologramMesh(StructureHolograms[1+i], bIsPlacementPossible);
			}

		}
	}

	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, FMath::Clamp(BoomArmTargetLength, 300, 10000),
	                                              GetWorld()->DeltaRealTimeSeconds, 10);
}

void APlayerPawn::Click(const FInputActionValue& InputActionValue)
{
	UpdateHitResultUnderCursor();

	const FVector Location = HitResultUnderCursor.Location;


	const AMapTile* Tile = Cast<AMapTile>(HitResultUnderCursor.GetActor());
	if (!Tile) { return; }

	// Try to place tower if a tower is selected
	if (PlayerActionState == Placing)
	{
		if (UStructureData* StructureData = Cast<UStructureData>(StructureToPlace))
		{
			if (GameState->AegisMap->AddStructureToMap(Tile->TileCoord, StructureData, this))
			{
				PlayerActionState = Default;
				StructureToPlace = nullptr;
				ClearStructureHolograms();
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

	// Ensure there are enough structure holograms
	ClearStructureHolograms();
	
	for (int i = 0; i < StructureData->StructureOffsets.Num()+1; i++)
	{
		//UStaticMeshComponent* StructureHologramComp = CreateDefaultSubobject<UStaticMeshComponent>("Hologram Structure" + i);
		UStaticMeshComponent* StructureHologramComp = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass());
		StructureHologramComp->RegisterComponent();
		
		StructureHologramComp->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		StructureHologramComp->SetVisibility(false);
		StructureHologramComp->SetCollisionResponseToAllChannels(ECR_Ignore);
		StructureHologramComp->SetWorldLocation(FTileCoord().ToWorldLocation());
		StructureHolograms.Add(StructureHologramComp);
		StructureHologramComp->SetVisibility(true);
	}
}
