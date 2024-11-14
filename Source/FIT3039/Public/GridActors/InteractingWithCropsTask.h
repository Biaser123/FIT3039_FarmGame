// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Crops.h"
#include "BehaviorTree/BTTaskNode.h"
#include "InteractingWithCropsTask.generated.h"

/**
 * 
 */
UCLASS()
class FIT3039_API UInteractingWithCropsTask : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UInteractingWithCropsTask();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

private:
	UFUNCTION()
	void OnInteractComplete();

	UPROPERTY()
	ACrop* Crop;

	UPROPERTY()
	UBehaviorTreeComponent* OwnerCompRef;

	FTimerHandle InteractTimerHandle;

	bool bIsBound;
};
