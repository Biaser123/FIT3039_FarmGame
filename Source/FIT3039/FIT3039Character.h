// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FIT3039/Public/CardStorage/Hand.h"
#include "FIT3039Character.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOpenShopDelegate);

UCLASS(Blueprintable)
class AFIT3039Character : public ACharacter
{
	GENERATED_BODY()

public:
	AFIT3039Character();	

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite,Category = Logic_Component)
	class UHand* Hand;

	UPROPERTY(EditAnywhere)
	float MovementSpeed;

	UFUNCTION(BlueprintCallable)
	void IncreaseGold(const int GoldNum);
	UFUNCTION(BlueprintCallable)
	void DecreaseGold(const int GoldNum);
	UFUNCTION(BlueprintCallable)
	int GetGold();

	void SetIsOverlappingWithShop(bool IsOverlapping);

	bool GetIsOverlappingWithShop();
	
	void SetIsOverlappingWithChargingStation(bool IsOverlapping);

	bool GetIsOverLappingWithChargingStation();

	void InteractWithShop();

	UPROPERTY(BlueprintAssignable)
	FOpenShopDelegate OpenShopDelegate;

	UAudioComponent* InteractionAudioComponent;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Audio")
	UAudioComponent* WalkAudioComponent;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Audio")
	USoundCue* ChargingSoundCue;
	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundCue* WateringSoundCue;
	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundCue* SeedingSoundCue;
	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundCue* HarvestingSoundCue;
	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundCue* WalkingSoundCue;

	UFUNCTION(BlueprintCallable)
	void PlayInteractionSound(USoundCue* SoundCue);
	UFUNCTION(BlueprintCallable)
	void StopInteractionSound();

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Audio")
	bool bIsPlaying = false;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Audio")
	bool bIsMovingPlaying = false;

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	// shop interact check
	bool bIsOverlappingWithShop = false;

	//
	bool bIsOverlappingWithChargingStation =false;
	
	int Gold;


};

