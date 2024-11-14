// Copyright Epic Games, Inc. All Rights Reserved.

#include "FIT3039Character.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"

AFIT3039Character::AFIT3039Character()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	//create hand
	Hand = CreateDefaultSubobject<UHand>(TEXT("Hand"));
	Gold = 10;
	
	
	// movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // auto orient
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	GetCharacterMovement()->MaxAcceleration = 10000.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 10000.f;

	//audio
	InteractionAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("InteractionAudioComponent"));
	InteractionAudioComponent->bAutoActivate = false;
	InteractionAudioComponent->SetupAttachment(RootComponent);

	WalkAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("WalkAudioComponent"));
	WalkAudioComponent->bAutoActivate = false;
	WalkAudioComponent->SetupAttachment(RootComponent);
	WalkAudioComponent->SetSound(WalkingSoundCue);
	//MovementSpeed = 400.f;
	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;


}

void AFIT3039Character::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}



int AFIT3039Character::GetGold()
{
	return Gold;
}

void AFIT3039Character::IncreaseGold(const int GoldNum)
{
	Gold += GoldNum;
}

void AFIT3039Character::DecreaseGold(const int GoldNum)
{
	Gold -= GoldNum;
}

void AFIT3039Character::SetIsOverlappingWithShop(bool IsOverlapping)
{
	bIsOverlappingWithShop = IsOverlapping;
}

bool AFIT3039Character::GetIsOverlappingWithShop()
{
	return bIsOverlappingWithShop;
}

void AFIT3039Character::SetIsOverlappingWithChargingStation(bool IsOverlapping)
{
	bIsOverlappingWithChargingStation = IsOverlapping;
}

bool AFIT3039Character::GetIsOverLappingWithChargingStation()
{
	return bIsOverlappingWithChargingStation;
}

void AFIT3039Character::InteractWithShop()
{
	if(bIsOverlappingWithShop)
	{
		OpenShopDelegate.Broadcast();
	}
}

void AFIT3039Character::PlayInteractionSound(USoundCue* SoundCue)
{
	if (InteractionAudioComponent && SoundCue && !bIsPlaying)
	{
		InteractionAudioComponent->SetSound(SoundCue);
		InteractionAudioComponent->Play();
		bIsPlaying = true;
		
	}
}

void AFIT3039Character::StopInteractionSound()
{
	if (InteractionAudioComponent && InteractionAudioComponent->IsPlaying())
	{
		InteractionAudioComponent->Stop();
		bIsPlaying = false;
	}
}
