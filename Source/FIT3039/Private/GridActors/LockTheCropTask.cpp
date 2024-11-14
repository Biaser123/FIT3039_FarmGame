// Fill out your copyright notice in the Description page of Project Settings.


#include "GridActors/LockTheCropTask.h"

#include "GridActors/PetAIController.h"


ULockTheCropTask::ULockTheCropTask()
{
	NodeName = "Lock The Crop";
}

EBTNodeResult::Type ULockTheCropTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	APetAIController* AIController = Cast<APetAIController>(OwnerComp.GetAIOwner());
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	ACrop* Crop = Cast<ACrop>(AIController->GetBlackboardComponent()->GetValueAsObject("TargetCrop"));
	
	if (Crop && !Crop->bIsPetInteracting)
	{
		Crop->bIsPetInteracting = true;
		return EBTNodeResult::Succeeded;
	}
	// if is already being interacted by other pet
	if(!Crop || Crop->bIsPetInteracting)
	{
		
		AIController->GetBlackboardComponent()->SetValueAsBool("FoundCrop", false);
		AIController->GetBlackboardComponent()->ClearValue("TargetCrop");
		AIController->TargetCrop = nullptr;
		return EBTNodeResult::Failed;
		
	}
	
	return EBTNodeResult::Failed;
}
