// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyBowlingCharacter.h"
#include "MyBowlingProjectile.h"
#include "Animation/AnimInstance.h"
//#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "Misc/OutputDeviceNull.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);


//////////////////////////////////////////////////////////////////////////
// AMyBowlingCharacter

void AMyBowlingCharacter::SetAimSensitivity(float Value)
{
	UWorld* World = GEngine->GameViewport->GetWorld();
	APlayerController* PC = GEngine->GetFirstLocalPlayerController(World);
	UPlayerInput* PlayerInput = PC->PlayerInput;

	PlayerInput->SetMouseSensitivity(Value);
}

AMyBowlingCharacter::AMyBowlingCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
/*	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));
*/
	// Create a gun mesh component
/*	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);*/

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
//	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);

	// Create a gun and attach it to the right-hand VR controller.
	// Create a gun mesh component
	/*VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	VR_Gun->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	VR_Gun->bCastDynamicShadow = false;
	VR_Gun->CastShadow = false;
	VR_Gun->SetupAttachment(R_MotionController);
	VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));*/

	VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
//	VR_MuzzleLocation->SetupAttachment(VR_Gun);
	VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.

	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;
}

void AMyBowlingCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
//	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	if (bUsingMotionControllers)
	{
//		VR_Gun->SetHiddenInGame(false, true);
//		Mesh1P->SetHiddenInGame(false, false);
	}
	else
	{
//		VR_Gun->SetHiddenInGame(true, true);
	//	Mesh1P->SetHiddenInGame(false, false);
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMyBowlingCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMyBowlingCharacter::OnFire);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AMyBowlingCharacter::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AMyBowlingCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyBowlingCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMyBowlingCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMyBowlingCharacter::LookUpAtRate);
}

void AMyBowlingCharacter::OnFire()
{
	/*// try and fire a projectile
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			if (bUsingMotionControllers)
			{
				const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
				const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();
				World->SpawnActor<AMyBowlingProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
			}
			else
			{
				const FRotator SpawnRotation = GetControlRotation();
				// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
				const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

				// spawn the projectile at the muzzle
				World->SpawnActor<AMyBowlingProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			}
		}
	}

	// try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}*/
}

void AMyBowlingCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AMyBowlingCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AMyBowlingCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick

//void AMyBowlingCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
//	{
//		if (TouchItem.bIsPressed)
//		{
//			if (GetWorld() != nullptr)
//			{
//				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
//				if (ViewportClient != nullptr)
//				{
//					FVector MoveDelta = Location - TouchItem.Location;
//					FVector2D ScreenSize;
//					ViewportClient->GetViewportSize(ScreenSize);
//					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
//					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.X * BaseTurnRate;
//						AddControllerYawInput(Value);
//					}
//					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.Y * BaseTurnRate;
//						AddControllerPitchInput(Value);
//					}
//					TouchItem.Location = Location;
//				}
//				TouchItem.Location = Location;
//			}
//		}
//	}
//}

void AMyBowlingCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AMyBowlingCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AMyBowlingCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMyBowlingCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool AMyBowlingCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AMyBowlingCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AMyBowlingCharacter::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AMyBowlingCharacter::TouchUpdate);
		return true;
	}
	
	return false;
}

void AMyBowlingCharacter::NextMove()
{
	NewMoveV = false;

	if (raw1 == true)
	{

		if (Player1Turn == true)
		{
			Palyer1SR1 = 0;
			Palyer1SR2 = 0;
			Palyer2SR1 = 0;
			Palyer2SR2 = 0;

			
		}
	}
	ScoreCalculate();


	if (raw1 == false)
	{
		
		if (Player1Turn == false)
		{
			NewMove();
		}
		Player1Turn = !Player1Turn;
	}	
	if (Moves == TotalMoves) 
	{
		EndGame();

	}
	

	raw1 = !raw1;



}

void AMyBowlingCharacter::EndGame()
{
	if (Payer2_ScoreTotal > Payer1_ScoreTotal)
	{
		Winner = "Player 2";
	}
	if (Payer1_ScoreTotal > Payer2_ScoreTotal)
	{
		Winner = "Player 1";
	}
	if (Payer1_ScoreTotal == Payer2_ScoreTotal)
	{
		Winner = "Draw";
	}

	FOutputDeviceNull Ar;
	const FString command = FString::Printf(TEXT("WinnerWid"));

	RulingActor->CallFunctionByNameWithArguments(*command, Ar, NULL, true);
}

void AMyBowlingCharacter::NewMove()
{
	Moves++;

	ElimTriggerEnabled = false;
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), PinClass, FoundActors);//	GetAllActors

	for (int i = 0; i < FoundActors.Num(); i++)
	{
		FoundActors[i]->Destroy();
	}
	ElimTriggerEnabled = true;


	FOutputDeviceNull Ar;
	const FString command = FString::Printf(TEXT("TableEdit"));

	RulingActor->CallFunctionByNameWithArguments(*command,Ar, NULL, true);

	SpawnActors();
}

void AMyBowlingCharacter::ScoreCalculate()
{
//	FString Count = FString::FromInt((Payer1_Score));
//	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, "Player SC  1 " + Count);
//	Count = FString::FromInt((Payer2_Score));
//	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, "Player SC 2  " + Count);

	if ((raw1 == true) && (Player1Turn == true))
	{
		Palyer1SR1 = StrikeMod1 * Payer1_Score;
		Payer1_ScoreTotal += Palyer1SR1;
		if ((Payer1_Score == 10) || (Payer1_Score == 20))
		{
			//Strike
			raw1 = false;
		//	Player1Turn = false;

			StrikeW();
			StrikeMod1 = 2;
		}

		}
	else if ((raw1 == false) && (Player1Turn == true))
	{
		Palyer1SR2 = StrikeMod1 * (Payer1_Score-Palyer1SR1);
		Payer1_ScoreTotal += Palyer1SR2;
		StrikeMod1 = 1;

	}
	else if ((raw1 == true) && (Player1Turn == false))
	{
		Palyer2SR1 = StrikeMod2* Payer2_Score;
		Payer2_ScoreTotal += Palyer2SR1;
		if ((Payer2_Score == 10) || (Payer2_Score == 20))
		{
			//Strike
			NewMove();
			StrikeW();
			raw1 = false;
			StrikeMod2 = 2;
		//	Player1Turn = true;
		}

	}
	else if (raw1 == false && Player1Turn == false)
	{
		Palyer2SR2 = StrikeMod2*(Payer2_Score - Palyer2SR1);
		Payer2_ScoreTotal +=  Palyer2SR2;
		StrikeMod2 = 1;
	}


}
void AMyBowlingCharacter::StrikeW()
{
	FOutputDeviceNull Ar;
	const FString command = FString::Printf(TEXT("StikeWid"));

	RulingActor->CallFunctionByNameWithArguments(*command, Ar, NULL, true);
	
}
void AMyBowlingCharacter::SpawnActors()
{
	FVector Location;
	Location = FVector(-480.f, 1650.f, 220.f);
		GetWorld()->SpawnActor<AActor>(PinClass, Location, FRotator(0.f));

	Location = FVector(-680.f, 1650.f, 220.f);
		GetWorld()->SpawnActor<AActor>(PinClass, Location, FRotator(0.f));

	Location = FVector(-880.f, 1650.f, 220.f);
		GetWorld()->SpawnActor<AActor>(PinClass, Location, FRotator(0.f));

	Location = FVector(-1080.f, 1650.f, 220.f);
		GetWorld()->SpawnActor<AActor>(PinClass, Location, FRotator(0.f));

	Location = FVector(-580.f, 1500.f, 220.f);
		GetWorld()->SpawnActor<AActor>(PinClass, Location, FRotator(0.f));

	Location = FVector(-780.f, 1500.f, 220.f);
		GetWorld()->SpawnActor<AActor>(PinClass, Location, FRotator(0.f));
	
	Location = FVector(-980.f, 1500.f, 220.f);
		GetWorld()->SpawnActor<AActor>(PinClass, Location, FRotator(0.f));

	Location = FVector(-680.f, 1350.f, 220.f);
		GetWorld()->SpawnActor<AActor>(PinClass, Location, FRotator(0.f));

	Location = FVector(-880.f, 1350.f, 220.f);
		GetWorld()->SpawnActor<AActor>(PinClass, Location, FRotator(0.f));


	Location = FVector(-780.f, 1200.f, 220.f);
		GetWorld()->SpawnActor<AActor>(PinClass, Location, FRotator(0.f));

		const float Shift = -1020.0;

//		FVector Location;
		Location = FVector(-480.f + Shift, 1650.f, 220.f);
		GetWorld()->SpawnActor<AActor>(PinClass, Location, FRotator(0.f));

		Location = FVector(-680.f + Shift, 1650.f, 220.f);
		GetWorld()->SpawnActor<AActor>(PinClass, Location, FRotator(0.f));

		Location = FVector(-880.f + Shift, 1650.f, 220.f);
		GetWorld()->SpawnActor<AActor>(PinClass, Location, FRotator(0.f));

		Location = FVector(-1080.f + Shift, 1650.f, 220.f);
		GetWorld()->SpawnActor<AActor>(PinClass, Location, FRotator(0.f));

		Location = FVector(-580.f + Shift, 1500.f, 220.f);
		GetWorld()->SpawnActor<AActor>(PinClass, Location, FRotator(0.f));

		Location = FVector(-780.f + Shift, 1500.f, 220.f);
		GetWorld()->SpawnActor<AActor>(PinClass, Location, FRotator(0.f));

		Location = FVector(-980.f + Shift, 1500.f, 220.f);
		GetWorld()->SpawnActor<AActor>(PinClass, Location, FRotator(0.f));

		Location = FVector(-680.f + Shift, 1350.f, 220.f);
		GetWorld()->SpawnActor<AActor>(PinClass, Location, FRotator(0.f));

		Location = FVector(-880.f + Shift, 1350.f, 220.f);
		GetWorld()->SpawnActor<AActor>(PinClass, Location, FRotator(0.f));


		Location = FVector(-780.f + Shift, 1200.f, 220.f);
		GetWorld()->SpawnActor<AActor>(PinClass, Location, FRotator(0.f));

}