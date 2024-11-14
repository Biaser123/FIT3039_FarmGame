// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "ChargingStation.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FQTEUIDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRayCastDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGameWinDelegate);
UCLASS()
class FIT3039_API AChargingStation : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChargingStation();

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FGameWinDelegate FGameWin;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Mesh")
	UStaticMeshComponent *StationMesh;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Mesh")
	UStaticMeshComponent *FuelTankMesh;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Mesh")
	UStaticMeshComponent *FuelTankSignMesh;
	
	UPROPERTY(BlueprintAssignable, Category="Events")
	FQTEUIDelegate QTEAddedEvent;
	UPROPERTY(BlueprintAssignable, Category="Events")
	FQTEUIDelegate QTERemovedEvent;
	UPROPERTY(BlueprintAssignable, Category="Events")
	FQTEUIDelegate QTEMissedEvent;
	UPROPERTY(BlueprintAssignable, Category="Events")
	FRayCastDelegate RayCastFindEvent;


	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="UI Component")
	float CurrentProgress;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI Component")
	float RepairProgressRatio;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI Component")
	UWidgetComponent* InteractUIComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI Component")
	UWidgetComponent* ProgressionWidgetComponent;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Trigger")
	UBoxComponent* DetectionSphere;
	

	//FuelTankTransparentMaterial is used for the fade in/ fade out effect
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Materials")
	UMaterialInstance * FuelTankTransparentMaterial;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Materials")
	UMaterialInterface* ChargingStationInteractingMaterial;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Materials")
	UMaterialInterface* ChargingStationNotInteractingMaterial;
	
	// Function called when a player begins overlapping with the detection sphere
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Function called when a player ends overlapping with the detection sphere
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void StartInteract();
	UFUNCTION(BlueprintCallable)
	void CancelInteract();
	UFUNCTION()
	void CompleteInteract();
	UFUNCTION(BlueprintCallable)
	void ShutInteract();

	UFUNCTION(BlueprintCallable)
	void EnableRayCast();
	UFUNCTION(BlueprintCallable)
	void DisableRayCast();

	UFUNCTION(BlueprintCallable)
	void FuelTankLoaded();
	UFUNCTION(BlueprintCallable)
	void FuelTankUsed();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ChargingStation")
	float ChargingProgressRatio;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= "ChargingStation")
	float InteractionTime;

	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= "ChargingStation")
	bool bIsCharging = false;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= "ChargingStation")
	int ChargingCount;
	bool bIsLoading = false;
	bool BisInteracting = false;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= "ChargingStation")
	bool bLoadedFuelTank = false;

	bool bIsInteractable = false;

	void CheckQTE();
	
	void SetQTEChecked(bool bChecked);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	AChargingStation* GetChargingStationFromRayCast(const FVector& StartPoint, const FVector& EndPoint) const;

	bool bIsRaycastingEnabled = false;

	FTimerHandle PositionTimerHandle;
	FTimerHandle OpacityTimerHandle;
	FTimerHandle TimerHandle_Interact;
	
	FTimerHandle FuelTankDisappearTimerHandle;




private:
	
	//function fade in/ fade out
	void IncreaseOpacity();
	void DecreaseOpacity();

	//Fuel tank transform
	void LowerFuelTank();
	void HidingFuelTank();

	float CurrentOpacity = 0.f;
	float PositionTimeElapsed = 0.f;
	float InitialZ;

	bool bQTEChecked;
  
	// fade in/ fade out material
	UMaterialInstanceDynamic* DynamicTransparentMaterial;

	void UpdateInteractProgress();

	FTimerHandle TimerHandle_CheckQTE;

	UStaticMesh* Stage1Mesh;
	UStaticMesh* Stage2Mesh;
	UStaticMesh* Stage3Mesh;
	UStaticMesh* Stage4Mesh;
	UStaticMesh* Stage5Mesh;
	UStaticMesh* Stage6Mesh;
	UStaticMesh* Stage7Mesh;
	UStaticMesh* Stage8Mesh;
	
	
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
