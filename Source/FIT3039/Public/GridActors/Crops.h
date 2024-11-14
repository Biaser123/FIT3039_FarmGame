// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridNode.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/WidgetComponent.h"
#include "FIT3039/Public/Cards/CardDataStruct.h"

#include "GameFramework/Pawn.h"

#include "Crops.generated.h"


// water icon delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWaitingIconEvent);

//Disease Icon
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDiseaseAdded, EPlantMutation, DiseaseType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDiseaseRemoved);

//water qte delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FQTEUIAddedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FQTEUIRemovedDelegetes);
// AI Delegate
DECLARE_DELEGATE(FOnInteractComplete);

// Harvesting Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHarvestingEvent);

class UGridNode;

UCLASS()
class FIT3039_API ACrop : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACrop();

	UPROPERTY(BlueprintAssignable, Category="Events")
	FWaitingIconEvent WaterIconEvent;
	UPROPERTY(BlueprintAssignable, Category="Events")
	FWaitingIconEvent HarvestingIconEvent;
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnHarvestingEvent IcePepperHarvestMaterialEvent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Crop")
	USkeletalMeshComponent *CropMesh;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Crop")
	float CurrentProgress = 0.f;  // get the interact progress

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Crop")
	float CropProgressRatio;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI Component")
	UWidgetComponent* ProgressionWidgetComponent;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI Component")
	UWidgetComponent* WaitingForInteractingIconComponent;
	
	// Disease Icon 
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI Component")
	UWidgetComponent* CropStatusIconComponent;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FQTEUIAddedDelegate  QTEAdded;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FQTEUIRemovedDelegetes  QTERemoved;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDiseaseAdded  DiseaseAdded;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDiseaseRemoved  DiseaseRemoved;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int Price;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int OriginalPrice;
	
	UFUNCTION(Blueprintable)
	void UpdateInteractProgress();

	// UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Growth")
	// EGrowthStage GrowthStage;

	// UFUNCTION(BlueprintCallable, Category = "Growth")
	// void SetGrowthStage(EGrowthStage NewGrowthStage);

	//  Interacting logic
	void StartInteract(AActor* Interactor);
	void CancelInteract(AActor* Interactor);
	void CompleteInteract();
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Interact")
	bool bIsInteractable;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Interact")
	bool bIsPetInteracting = false;
	
	// growing logic
	bool bIsGrowing = false;

	UPROPERTY(BlueprintReadWrite, Category = "Crop")
	bool bIsBeingInteractedWith;
	
	float CurrentGrowProgress = 0.0f;

	
	float GrowProgressRate = 0.0f;  // grow rate

	void StartGrowthCycle();
	void CompleteGrowthCycle();

	FOnInteractComplete OnInteractComplete;

	//water logic
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crop")
	float WaterValue = 1.0f; // default full

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rate Value")
	float WaterDecreaseRate; // 0.2 decrease per sec

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crop")
	float WaterGrowthRateModifier = 1.0f; // the growth ratio depending on the water value

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crop")
	float DiseaseGrowthRateModifier = 1.0f;  // the growth ratio depending on the Disease value

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crop")
	float DisasterGrowthRateModifier = 1.0f;  // the growth ratio depending on the disaster
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crop")
	float WaterMutationRateModifier = 1.0f; // the water ratio depending on the mutation

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crop")
	float WaterDisasterRateModifier = 1.0f;  // the water ratio depending on the disaster

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Crop")
	float OxygenValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Crop")
	float TemperatureValue;

		
	UGridNode* GetOccupyingNode() const;

	void SetOccupyingNode(class UGridNode *Node);

	//mutation logic
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	TArray<FDiseaseProbability> MutationProbabilities;

	// Mutation value that increases in phase3
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mutation")
	float MutationValue = 0.f;

	// Mutation increase rate (editable in Blueprint)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rate Value")
	float MutationRate = 0.05;

	// Function to handle mutation updates
	void UpdateMutation(float DeltaTime);

	// Function to apply diseases based on probabilities
	void ApplyDiseases();

	UFUNCTION(BlueprintCallable)
	void ClearDiseases();

	bool bCouldMutation;

	// To store current diseases affecting the crop
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category = "Health")
	TSet<EPlantMutation> CurrentDiseases;

	bool HasDisease(EPlantMutation DiseaseType) const;

	void GetNeighborsNodes();

	UFUNCTION(BlueprintCallable)
	int GetMutationCount();
	
protected:
	float SeedingTime;  // seeding time

	float WateringTime;

	float FertilizingTime;

	float ReapingTime;

	UPROPERTY(BlueprintReadWrite, Category = "Rate Value")
	float GrowTime = 10.f;

	UPROPERTY(BlueprintReadWrite)
	FName RowName;
	
	bool bIsInteracting;  // check if its interacted

	UPROPERTY(BlueprintReadOnly, Category = "Grid")
	class UGridNode* OccupyNode;

	//save the nearby grids
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TArray<FGridVector> InfectedNeighbors;

	//infect logic
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	bool bIsInfected = false;
private:
	FTimerHandle TimerHandle_Interact;
	
	float InteractionTime = 0.f;

	int32 InteractingPlayerCount = 0;
	
	TSet<AActor*> InteractingActors;
	
	void ApplyDisasterEffects(EDisasterType DisasterType);
	
	//infect prob per sec
	float InfectionSpreadChance = 0.5f;

	void InfectAffectApplied();

	bool bIsFertilized = false;
	
	

	
};
