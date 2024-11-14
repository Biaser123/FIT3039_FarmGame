// Fill out your copyright notice in the Description page of Project Settings.


#include "GridActors/GenerateNewRandomLocationTask.h"

#include "GridActors/PetAIController.h"

EBTNodeResult::Type UGenerateNewRandomLocationTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBehaviorTreeComponent* Component = &OwnerComp;
	if(!Component)
		return EBTNodeResult::Failed;
	APetAIController* MyController = Cast<APetAIController>(Component->GetOwner());
	if(!MyController)
		return EBTNodeResult::Failed;
	MyController->GenerateNewRandomLocation();
	return EBTNodeResult::Succeeded;
}
