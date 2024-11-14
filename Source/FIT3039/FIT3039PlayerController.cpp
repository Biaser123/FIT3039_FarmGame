// Copyright Epic Games, Inc. All Rights Reserved.

#include "FIT3039PlayerController.h"

#include "ChargingStation.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "FIT3039Character.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GridActors/GridMap.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AFIT3039PlayerController::AFIT3039PlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	bQTETriggered = false;
	
}

void AFIT3039PlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	

	//Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
}


void AFIT3039PlayerController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	Super::SetupInputComponent();
	
	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	
	// Set up action bindings
	if (Input)
	{
		// Setup mouse input events
		Input->BindAction(TopAction, ETriggerEvent::Started, this, &AFIT3039PlayerController::MoveTopStart);
		Input->BindAction(TopAction, ETriggerEvent::Completed, this, &AFIT3039PlayerController::MoveTopEnd);
		
		Input->BindAction(DownAction, ETriggerEvent::Started, this, &AFIT3039PlayerController::MoveDownStart);
		Input->BindAction(DownAction, ETriggerEvent::Completed, this, &AFIT3039PlayerController::MoveDownEnd);
		
		Input->BindAction(LeftAction, ETriggerEvent::Started, this, &AFIT3039PlayerController::MoveLeftStart);
		Input->BindAction(LeftAction, ETriggerEvent::Completed, this, &AFIT3039PlayerController::MoveLeftEnd);
		
		Input->BindAction(RightAction, ETriggerEvent::Started, this, &AFIT3039PlayerController::MoveRightStart);
		Input->BindAction(RightAction, ETriggerEvent::Completed, this, &AFIT3039PlayerController::MoveRightEnd);

		Input->BindAction(InteractAction, ETriggerEvent::Started, this, &AFIT3039PlayerController::InteractStart);
		Input->BindAction(InteractAction, ETriggerEvent::Completed, this, &AFIT3039PlayerController::InteractEnd);

		Input->BindAction(CheckAction,ETriggerEvent::Triggered,this,&AFIT3039PlayerController::CheckQTE);

		Input->BindAction(ShopAction,ETriggerEvent::Triggered,this,&AFIT3039PlayerController::InteractWithShop);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AFIT3039PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	PossessedCharacter = Cast<AFIT3039Character>(InPawn);
	if(PossessedCharacter)
	{
		SetupPlayerInputComponent(InputComponent);
	}

}

void AFIT3039PlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(!MovementInput.IsZero() && PossessedCharacter)
	{
		FVector NormalisedMovement = MovementInput.GetSafeNormal();
		//FVector CurrentLocation = PossessedCharacter->GetActorLocation();
		
		FRotator NewRotation = NormalisedMovement.Rotation();
		PossessedCharacter->SetActorRotation(NewRotation);
		
		//FVector NextLocation = CurrentLocation + NormalisedMovement * PossessedCharacter->MovementSpeed * DeltaSeconds;
		//PossessedCharacter->SetActorLocation(NextLocation);
		
		PossessedCharacter->AddMovementInput(NormalisedMovement, 1.0f, false);
	}
}


void AFIT3039PlayerController::MoveRightStart()
{
	MovementInput.Y += 1;
	SavedMovementInput.Y += 1;
	
	CancelInteractIfMoving();
}

void AFIT3039PlayerController::MoveRightEnd()
{
	MovementInput.Y -= 1;
	SavedMovementInput.Y -= 1;
	
}

void AFIT3039PlayerController::MoveLeftStart()
{
	
	MovementInput.Y -= 1;
	SavedMovementInput.Y -= 1;
	CancelInteractIfMoving();
}

void AFIT3039PlayerController::MoveLeftEnd()
{
	MovementInput.Y += 1;
	SavedMovementInput.Y += 1;
}

void AFIT3039PlayerController::MoveTopStart()
{
	MovementInput.X += 1;
	SavedMovementInput.X += 1;
	CancelInteractIfMoving();
}

void AFIT3039PlayerController::MoveTopEnd()
{
	MovementInput.X -= 1;
	SavedMovementInput.X -= 1;
}

void AFIT3039PlayerController::MoveDownStart()
{
	MovementInput.X -= 1;
	SavedMovementInput.X -= 1;
	
	CancelInteractIfMoving();
}

void AFIT3039PlayerController::MoveDownEnd()
{
	MovementInput.X += 1;
	SavedMovementInput.X += 1;
}

void AFIT3039PlayerController::InteractStart()
{
	
	AFIT3039Character* PlayerCharacter = Cast<AFIT3039Character>(GetPawn());
	if (PlayerCharacter && PlayerCharacter->GetIsOverLappingWithChargingStation())
	{
		// Find all charging station instances in the world and trigger interaction
		for (TActorIterator<AChargingStation>It(GetWorld()); It; ++It)
		{
			AChargingStation* ChargingStation = *It;
			if (ChargingStation)
			{
				if(ChargingStation->bLoadedFuelTank){
					InteractChargingStation = ChargingStation;
					bIsInteractingWithChargingStation = true;
					
					GetWorldTimerManager().SetTimer(TimerHandle_LongPress, this, &AFIT3039PlayerController::CheckForChargingInteraction, 1.0f / 60.0f, true);
					break; 
				}
			}
		}
	}
	else
	{
		bIsInteractingWithGrid = true;
		GetWorldTimerManager().SetTimer(TimerHandle_LongPress, this, &AFIT3039PlayerController::CheckForGridInteraction, 1.0f / 60.0f, true);
	}
}

void AFIT3039PlayerController::CheckQTE()
{
	//UE_LOG(LogTemp, Warning, TEXT("CheckQTE called, bQTETriggered is: %s"), bQTETriggered ? TEXT("true") : TEXT("false"));
	
	if(LastInteractedCrop != nullptr && !bQTETriggered)
		{
			if (LastInteractedCrop->Tags.Contains(TEXT("Watering")))
			{
				OnQTETriggered.Broadcast();
				bQTETriggered = true;
				GetWorld()->GetTimerManager().SetTimer(UnusedHandle, this, &AFIT3039PlayerController::ResetQTE, 0.2f, false);
			}
		}
	else if (InteractChargingStation != nullptr && !bQTETriggered)
	{
		bQTETriggered = true;
		RepairingQTETriggered.Broadcast();
		InteractChargingStation->SetQTEChecked(true);
		GetWorld()->GetTimerManager().SetTimer(UnusedHandle, this, &AFIT3039PlayerController::ResetQTE, 0.5f, false);
		return;
	}
	
}

void AFIT3039PlayerController::ResetQTE()
{
	bQTETriggered = false;
	GetWorld()->GetTimerManager().ClearTimer(UnusedHandle);
}


void AFIT3039PlayerController::InteractEnd()
{
	if (bIsInteractingWithGrid)
	{
		PossessedCharacter->Tags.Remove("Interacting");
		PossessedCharacter->Tags.Remove("Watering");
		bIsInteractingWithGrid = false;
		bWaterNiagaraTriggered = false;
		OnWaterNiagaraEnded.Broadcast();
		AFIT3039Character* ControlledPawn =  Cast<AFIT3039Character>(GetPawn());
		ControlledPawn->StopInteractionSound();

		// call the cancel function
		if (LastInteractedCrop)
		{
			LastInteractedCrop->CancelInteract(this);
			LastInteractedCrop = nullptr;  // reset
			MovementInput = SavedMovementInput;
		}
	}
	else if(bIsInteractingWithChargingStation)
	{
		PossessedCharacter->Tags.Remove("Interacting");
		InteractChargingStation ->CancelInteract();
		bIsInteractingWithChargingStation = false;
		MovementInput = SavedMovementInput;
		AFIT3039Character* ControlledPawn =  Cast<AFIT3039Character>(GetPawn());
		ControlledPawn->StopInteractionSound();
	}
	GetWorldTimerManager().ClearTimer(TimerHandle_LongPress);
}


void AFIT3039PlayerController::CheckForGridInteraction()
{
    if (bIsInteractingWithGrid)
    {
        AFIT3039Character* ControlledPawn =  Cast<AFIT3039Character>(GetPawn());
        if (ControlledPawn)
        {
            TArray<UPrimitiveComponent*> OverlappingComponents;
            ControlledPawn->GetOverlappingComponents(OverlappingComponents);

            ACrop* NearestCrop = nullptr;
            float NearestDistance = TNumericLimits<float>::Max();

            for (UPrimitiveComponent* Component : OverlappingComponents)
            {
            	//check the overlap grid first
                UBoxComponent* CollisionComponent = Cast<UBoxComponent>(Component);
                if (CollisionComponent)
                {
                    AActor* OverlappingActor = CollisionComponent->GetOwner();
                    AGridMap* GridMap = Cast<AGridMap>(OverlappingActor);
                    if (GridMap)
                    {
                        UGridNode* PlayerGridNode = GridMap->CollisionComponentToNodeMap.FindRef(CollisionComponent);
                        if (PlayerGridNode)
                        {
                            ACrop* Crop = PlayerGridNode->GetOccupyingPlant();
                            if (Crop && Crop->bIsInteractable)
                            {
                                float Distance = (Crop->GetActorLocation() - ControlledPawn->GetActorLocation()).Size();
                                if (Distance < NearestDistance)
                                {
                                	//get the nearest crop from the list of overlapping grid
                                    NearestCrop = Crop;
                                    NearestDistance = Distance;
                                }
                            }
                        }
                    }
                }
            }

            if (NearestCrop)
            {
                FVector TargetDirection = NearestCrop->GetActorLocation() - ControlledPawn->GetActorLocation();
                TargetDirection.Z = 0.0f; // Ignore Z
                TargetDirection.Normalize();

                FRotator TargetRotation = TargetDirection.Rotation();
                ControlledPawn->SetActorRotation(TargetRotation);
                NearestCrop->StartInteract(this);

            	//set movement to 0 and save the original movement
            	
            	MovementInput = FVector::ZeroVector;
                LastInteractedCrop = NearestCrop;
            	if(LastInteractedCrop->ActorHasTag("Watering") && !bWaterNiagaraTriggered)
            	{
            		PossessedCharacter->Tags.Add("Watering");
            		OnWaterNiagaraTriggered.Broadcast();
            		bWaterNiagaraTriggered = true;
            		ControlledPawn->PlayInteractionSound(ControlledPawn->WateringSoundCue);
            		
            	}
            	else if(LastInteractedCrop->ActorHasTag("Seeding"))
            	{
            		ControlledPawn->PlayInteractionSound(ControlledPawn->SeedingSoundCue);
            		UE_LOG(LogTemp, Log, TEXT("Playing Seeding sound"))
            		
            	}
            	else if(LastInteractedCrop->ActorHasTag("Reaping"))
            	{
            		ControlledPawn->PlayInteractionSound(ControlledPawn->HarvestingSoundCue);
            		
            	}

            	//for animation
            	if(!LastInteractedCrop->ActorHasTag("Watering"))
            	{
            		PossessedCharacter->Tags.Add("Interacting");
            		
            	}
            	
            }
        }
    }
}

void AFIT3039PlayerController::CheckForChargingInteraction()
{
	AFIT3039Character* ControlledPawn =  Cast<AFIT3039Character>(GetPawn());
    if (bIsInteractingWithChargingStation && InteractChargingStation)
    {
    	if (InteractChargingStation-> bIsCharging != true && InteractChargingStation-> bIsInteractable == true)
    	{
    		InteractChargingStation->StartInteract();
    		ControlledPawn->PlayInteractionSound(ControlledPawn->ChargingSoundCue);
    		//set movement to 0 and save the original movement
    		MovementInput = FVector::ZeroVector;
    	}
    }
}

ACrop* AFIT3039PlayerController::GetLastInteractedCrop()
{
	return LastInteractedCrop;
}

AChargingStation* AFIT3039PlayerController::GetChargingStation()
{
    return InteractChargingStation;
}


void AFIT3039PlayerController::CancelInteractIfMoving()
{
	// stop interact when moving 
	if (!MovementInput.IsZero() && bIsInteractingWithGrid)
	{
		AFIT3039Character* ControlledPawn =  Cast<AFIT3039Character>(GetPawn());
		ControlledPawn->StopInteractionSound();
		bIsInteractingWithGrid = false;
		bWaterNiagaraTriggered = false;
		OnWaterNiagaraEnded.Broadcast();
		PossessedCharacter->Tags.Remove("Interacting");
		PossessedCharacter->Tags.Remove("Watering");

		// call the cancel function
		if (LastInteractedCrop)
		{
			LastInteractedCrop->CancelInteract(this);
			MovementInput = SavedMovementInput;
			LastInteractedCrop = nullptr;  // reset
		}
		GetWorldTimerManager().ClearTimer(TimerHandle_LongPress);
	}
	if (!MovementInput.IsZero() && bIsInteractingWithChargingStation)
	{
		PossessedCharacter->Tags.Remove("Interacting");
		bIsInteractingWithChargingStation = false;

		// call the cancel function
		if (InteractChargingStation)
		{
			InteractChargingStation->CancelInteract();
			AFIT3039Character* ControlledPawn =  Cast<AFIT3039Character>(GetPawn());
			ControlledPawn->StopInteractionSound();
			MovementInput = SavedMovementInput;
			InteractChargingStation = nullptr;  // reset
		}
		GetWorldTimerManager().ClearTimer(TimerHandle_LongPress);
	}
}

void AFIT3039PlayerController::InteractWithShop()
{
	
	AFIT3039Character* MyCharacter = Cast<AFIT3039Character>(GetPawn());
	
	if(MyCharacter)
	{
		MyCharacter->InteractWithShop();
	}
	
}



