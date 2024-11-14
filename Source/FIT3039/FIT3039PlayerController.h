// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FIT3039Character.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "EnhancedInputSubsystems.h"

#include "EngineUtils.h"
#include "ChargingStation.h"
#include "FIT3039/Public/GridActors/GridNode.h"

#include "FIT3039PlayerController.generated.h"

/** Forward declaration to improve compiling times */
class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnQTETriggeredDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWaterEffectDelegate);

UCLASS()
class AFIT3039PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AFIT3039PlayerController();
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;
	
	UPROPERTY(EditAnywhere)
	UInputAction* RightAction;
	UPROPERTY(EditAnywhere)
	UInputAction* LeftAction;
	UPROPERTY(EditAnywhere)
	UInputAction* TopAction;
	UPROPERTY(EditAnywhere)
	UInputAction* DownAction;
	UPROPERTY(EditAnywhere)
	UInputAction* InteractAction;
	UPROPERTY(EditAnywhere)
	UInputAction* CheckAction;
	UPROPERTY(EditAnywhere)
	UInputAction* ShopAction;
	
	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent);

	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaSeconds) override;

	FVector MovementInput;
	AFIT3039Character* PossessedCharacter;
	
	// To add mapping context
	virtual void BeginPlay();

	/** Input handlers for SetDestination action. */
	void CancelInteractIfMoving();
	
	void MoveRightStart();
	void MoveRightEnd();
	void MoveLeftStart();
	void MoveLeftEnd();
	void MoveTopStart();
	void MoveTopEnd();
	void MoveDownStart();
	void MoveDownEnd();

	void InteractStart();
	void InteractEnd();

	void InteractWithShop();



	//Qte Check method and the delegate for it
	void CheckQTE();

	// qte for watering
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnQTETriggeredDelegate  OnQTETriggered;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnWaterEffectDelegate OnWaterNiagaraTriggered;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnWaterEffectDelegate OnWaterNiagaraEnded;

	//qte for Repairing
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnQTETriggeredDelegate  RepairingQTETriggered;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bQTETriggered;
	
	FTimerHandle UnusedHandle;
	
	void ResetQTE();
	
	void CheckForGridInteraction();
	void CheckForChargingInteraction();

	UFUNCTION(BlueprintCallable)
	ACrop* GetLastInteractedCrop();

	UFUNCTION(BlueprintCallable)
	AChargingStation* GetChargingStation();

	
	
private:
	bool bIsInteractingWithGrid = false;
	bool bIsInteractingWithChargingStation = false;
	FTimerHandle TimerHandle_LongPress;

	ACrop* LastInteractedCrop = nullptr;
	AChargingStation* InteractChargingStation = nullptr;

	FVector SavedMovementInput;
	bool bIsInteracting = false;
	bool bWaterNiagaraTriggered = false;
};


