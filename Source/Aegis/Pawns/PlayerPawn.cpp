// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPawn.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Aegis/AegisGameStateBase.h"
#include "Aegis/Map/MapTile.h"
#include "Aegis/Structures/Structure.h"
#include "Aegis/Structures/Towers/Tower.h"
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

	FocusPoint = CreateDefaultSubobject<UStaticMeshComponent>("Focus Point");
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
	//UMaterial* DecalMat = ConstructorHelpers::FObjectFinder<UMaterial>(TEXT("/Script/Engine.Material'/Game/Aegis/Art/Decals/M_RangeDecal_Rotating.M_RangeDecal_Rotating'")).Object;
	UMaterial* DecalMat = ConstructorHelpers::FObjectFinder<UMaterial>(TEXT("/Script/Engine.Material'/Game/Aegis/Art/Decals/M_RangeDecal_Static.M_RangeDecal_Static'")).Object;
	RangeIndicatorDecal->SetMaterial(0, DecalMat);

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

	
}

void APlayerPawn::ClearStructureHolograms()
{
	for(UStaticMeshComponent* Component : StructureHolograms)
	{
		Component->DestroyComponent();
	}
	StructureHolograms = TArray<UStaticMeshComponent*>();
}

void APlayerPawn::SelectStructure(AStructure* StructureToSelect)
{
	// What to do to unselect the old tower
	if (const ATower* Tower = Cast<ATower>(SelectedStructure))
	{
		Tower->RangeIndicatorDecal->SetVisibility(false);
	}
	
	SelectedStructure = StructureToSelect;
	
	if (const ATower* Tower = Cast<ATower>(SelectedStructure))
	{
		Tower->RangeIndicatorDecal->SetVisibility(true);
	}
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

	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, FMath::Clamp(BoomArmTargetLength, 300, 10000),
	                                              GetWorld()->DeltaRealTimeSeconds, 10);
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
