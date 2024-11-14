// Fill out your copyright notice in the Description page of Project Settings.


#include "GridActors/InteractingWithCropsTask.h"

#include "GridActors/LockTheCropTask.h"
#include "GridActors/PetAIController.h"


UInteractingWithCropsTask::UInteractingWithCropsTask()
    : Crop(nullptr), OwnerCompRef(nullptr), bIsBound(false)
{
    NodeName = "Interact With Crop";

    //use instance to deal with binding 
    bCreateNodeInstance = true;
}

EBTNodeResult::Type UInteractingWithCropsTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    APetAIController* AIController = Cast<APetAIController>(OwnerComp.GetAIOwner());
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }
    APawn* PetPawn = AIController->GetPawn(); // get pet
    Crop = AIController->TargetCrop;
    if (Crop && PetPawn && Crop->bIsInteractable)
    {
        OwnerCompRef = &OwnerComp; 
        if (!bIsBound)
        {
            Crop->OnInteractComplete.BindUObject(this, &UInteractingWithCropsTask::OnInteractComplete);
            UE_LOG(LogTemp, Warning, TEXT("bind complete：PetPawn %s Task %s bind to Crop %s"), *PetPawn->GetName(), *this->GetName(), *Crop->GetName());
            bIsBound = true;
        }
        Crop->StartInteract(PetPawn);// pass the interactor

        // save a nd use the ref in OnInteractComplete
        
        return EBTNodeResult::InProgress;
    }

    return EBTNodeResult::Failed;
}

void UInteractingWithCropsTask::OnInteractComplete() 
{
    
    if(Crop)
    {
        APetAIController* AIController = Cast<APetAIController>(OwnerCompRef->GetAIOwner());
        if (AIController)
        {
            Crop->OnInteractComplete.Unbind();
            UE_LOG(LogTemp, Warning, TEXT("unBindComplete：Task %s unbind to Crop %s"), *this->GetName(), *Crop->GetName());
            AIController->RemoveCropFromDetected(Crop);
            AIController->GetBlackboardComponent()->ClearValue("TargetCrop");
            AIController->GetBlackboardComponent()->SetValueAsBool("FoundCrop",false);
            AIController->TargetCrop = nullptr;
            Crop = nullptr;
            bIsBound = false;
        }
        FinishLatentTask(*OwnerCompRef, EBTNodeResult::Succeeded); // turn to succeeded
    
    }
    
}


void UInteractingWithCropsTask::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
    
    
    Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}
