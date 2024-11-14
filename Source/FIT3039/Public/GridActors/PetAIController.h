// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIController.h"
#include "Crops.h"
#include "NavigationSystem.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "PetAIController.generated.h"

/**
 * 
 */
UCLASS()
class FIT3039_API APetAIController : public AAIController
{
	GENERATED_BODY()
	APetAIController();


	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual FRotator GetControlRotation() const override;

	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=AI)
	float SightRadius = 1000;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=AI)
	float SightAge = 3.5;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=AI)
	float LoseSightRadius = SightRadius + 30;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=AI)
	float FieldOfView = 360;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=AI)
	UAISenseConfig_Sight* SightConfiguration;
	
	UNavigationSystemV1* NavigationSystem;

	UPROPERTY(BlueprintReadOnly)
	ACrop* TargetCrop;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=AI)
	FName RequiredTag;

	void GenerateNewRandomLocation();

	UPROPERTY(BlueprintReadOnly)
	TArray<ACrop*> DetectedCrops;
	UPROPERTY(BlueprintReadOnly)
	TArray<ACrop*> SeenCrops;

	UFUNCTION()
	void OnSensesUpdated(AActor* UpdatedActor, FAIStimulus Stimulus);

	UPROPERTY(EditDefaultsOnly, Category = Blackboard)
     UBlackboardData* AIBlackboard;
    UPROPERTY(EditDefaultsOnly, Category = Blackboard)
     UBehaviorTree* BehaviorTree;
    UPROPERTY(EditDefaultsOnly, Category = Blackboard)
     UBlackboardComponent* BlackboardComponent;

	void UpdateNearestCrop();

	void RemoveCropFromDetected(ACrop* CropActor);

private:
	bool bShouldUpdateNearestCrop = false;
	const int MaxAttempts = 100;
	int AttemptCount = 0;

	bool bPointFound = false;

	
};
