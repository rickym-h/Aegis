// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPawn.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Aegis/AegisGameStateBase.h"
#include "Aegis/Map/MapTile.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
#include "Input/InputConfigData.h"

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

	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>("Floating Movement Component");

	
}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	if (Cast<AAegisGameStateBase>(GetWorld()->GetGameState()))
	{
		GameState = Cast<AAegisGameStateBase>(GetWorld()->GetGameState());
	}
}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlayerPawn::Click(const FInputActionValue& InputActionValue)
{
	//UE_LOG(LogTemp, Warning, TEXT("CLICK"))

	const APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (!PlayerController) { return; }
	
	FHitResult HitResult; 
	if (PlayerController->GetHitResultUnderCursorByChannel(TraceTypeQuery1, false, HitResult))
	{
		if (const AMapTile* Tile = Cast<AMapTile>(HitResult.GetActor()))
		{
			UE_LOG(LogTemp, Warning, TEXT("TileCoord: %ls"), *Tile->TileCoord.ToString())

			GameState->AegisMap->AddDefenderToMap(Tile->TileCoord);
		}
	}

	DrawDebugSphere(GetWorld(), HitResult.Location, 5, 12, FColor::Red, false, 2, 0, 1);
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

