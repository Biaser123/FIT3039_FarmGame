// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "LockTheCropTask.generated.h"

/**
 * 
 */
UCLASS()
class FIT3039_API ULockTheCropTask : public UBTTaskNode
{
	GENERATED_BODY()

	ULockTheCropTask();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
