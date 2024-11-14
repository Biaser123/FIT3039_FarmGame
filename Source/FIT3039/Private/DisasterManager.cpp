// Fill out your copyright notice in the Description page of Project Settings.


#include "DisasterManager.h"

#include "DisasterManager.h"
#include "FIT3039/Public/GridActors/Crops.h"
#include "Kismet/GameplayStatics.h"

#include "TimerManager.h"

// Assume this is your crop class

ADisasterManager::ADisasterManager()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ADisasterManager::BeginPlay()
{
    Super::BeginPlay();
    CachedTimeManager = Cast<ATimeManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ATimeManager::StaticClass()));
    CachedBalancingValue = Cast<ABalancingValue>(UGameplayStatics::GetActorOfClass(GetWorld(),ABalancingValue::StaticClass()));
    
}



void ADisasterManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (CachedTimeManager && CachedTimeManager->TotalDays >= 2 && FMath::IsNearlyEqual(CachedTimeManager->CurrentHour, 12.0f, 0.1f))
    {
        if (!bIsDisasterHappening)
        {
            CheckForDisaster();
        }
    }
}

void ADisasterManager::CheckForDisaster()
{
    if (FMath::RandRange(0.0f, 1.0f) < DisasterChance && CachedTimeManager)
    {
        // this bool is to ensure not checking when already have one
        bIsDisasterHappening = true;
        CurrentDisasterType = static_cast<EDisasterType>(FMath::RandRange(0, 2));
        float triggerTime = CachedTimeManager->RealSecondsPerGameHour * 20;
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ADisasterManager::TriggerDisaster, triggerTime, false);
        OnDisasterWarning.Broadcast(CurrentDisasterType);
    }
}

void ADisasterManager::TriggerDisaster()
{
    OnDisasterStarted.Broadcast(CurrentDisasterType);
    //this bool is to tell crops a Disaster is happening and got the affect when the player planting a new crop during the disaster
    bIsDisasterActive = true;
    ApplyDisasterEffects(CurrentDisasterType);
    float Duration = GetDisasterDuration(CurrentDisasterType);

    // Set a timer to end the disaster after the duration specified by GetDisasterDuration
    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]() {
        ClearDisasterEffects(CurrentDisasterType);}, Duration, false);
}

void ADisasterManager::ApplyDisasterEffects(EDisasterType Disaster)
{
    if (Disaster == EDisasterType::Drought)
    {
        TArray<AActor*> FoundCrops;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACrop::StaticClass(), FoundCrops);
        for (AActor* Actor : FoundCrops)
        {
            ACrop* Crop = Cast<ACrop>(Actor);
            if (Crop)
            {
                Crop->WaterDisasterRateModifier *= 2;
            }
        }
        if(CachedBalancingValue)
        {
            CachedBalancingValue->BaseTemperature += 5.0f;
        }
    }
    else if (Disaster == EDisasterType::ColdSnap)
    {
        TArray<AActor*> FoundCrops;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACrop::StaticClass(), FoundCrops);
        for (AActor* Actor : FoundCrops)
        {
            ACrop* Crop = Cast<ACrop>(Actor);
            if (Crop)
            {
                Crop->DisasterGrowthRateModifier *= 1.2;
            }
        }
        if(CachedBalancingValue)
        {
            CachedBalancingValue->BaseTemperature -= 5.0f;
        }
        
    }
    else if (Disaster == EDisasterType::InfectiousDisease)
    {
        // add the infectious to the crops mutation arrays
        FDiseaseProbability InfectiousDiseaseProbability;
        InfectiousDiseaseProbability.MutationType = EPlantMutation::PM_InfectiousDisease;
        InfectiousDiseaseProbability.Probability = 0.01f; 
        TArray<AActor*> FoundCrops;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACrop::StaticClass(), FoundCrops);
        for (AActor* Actor : FoundCrops)
        {
            ACrop* Crop = Cast<ACrop>(Actor);
            if (Crop)
            {
                Crop->MutationProbabilities.Add(InfectiousDiseaseProbability);
            }
        }
        
    }
}

void ADisasterManager::ClearDisasterEffects(EDisasterType Disaster)
{
    if (OnDisasterEnded.IsBound())
    {
        OnDisasterEnded.Broadcast(CurrentDisasterType);
    }
    bIsDisasterHappening = false;
    bIsDisasterActive = false;
    // Only clear effects for the specified disaster type
    if (Disaster ==  EDisasterType::Drought)
    {
        TArray<AActor*> FoundCrops;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACrop::StaticClass(), FoundCrops);
        for (AActor* Actor : FoundCrops)
        {
            ACrop* Crop = Cast<ACrop>(Actor);
            if (Crop)
            {
                Crop->WaterDisasterRateModifier = 1.0f;
            }
        }
        if(CachedBalancingValue)
        {
            CachedBalancingValue->BaseTemperature -= 5.0f;
        }
    }
    else if (Disaster == EDisasterType::ColdSnap)
    {
        TArray<AActor*> FoundCrops;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACrop::StaticClass(), FoundCrops);
        for (AActor* Actor : FoundCrops)
        {
            ACrop* Crop = Cast<ACrop>(Actor);
            if (Crop)
            {
                Crop->DisasterGrowthRateModifier = 1.0f;
            }
        }
        if(CachedBalancingValue)
        {
            CachedBalancingValue->BaseTemperature += 5.0f;
        }
    }
    
    else if (Disaster == EDisasterType::InfectiousDisease)
    {
        TArray<AActor*> FoundCrops;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACrop::StaticClass(), FoundCrops);

        // Define a lambda function to check if an element matches InfectiousDiseaseProbability
        auto Predicate = [&](const FDiseaseProbability& Item) {
            return Item.MutationType == EPlantMutation::PM_InfectiousDisease && Item.Probability == 0.3f;
        };

        // Iterate over FoundCrops and remove elements matching InfectiousDiseaseProbability
        for (AActor* Actor : FoundCrops)
        {
            ACrop* Crop = Cast<ACrop>(Actor);
            if (Crop)
            {
                // Check if InfectiousDiseaseProbability is already present
                if (Crop->MutationProbabilities.ContainsByPredicate(Predicate))
                {
                    // If present, remove it
                    Crop->MutationProbabilities.RemoveAll(Predicate);
                }
            }
        }
        
    }
    
}

float ADisasterManager::GetDisasterDuration(EDisasterType Disaster)
{
    float DurationInGameHours;  // Disaster duration time

    switch (Disaster)
    {
    case EDisasterType::Drought:
        DurationInGameHours = 18;
        break;
    case EDisasterType::ColdSnap:
        DurationInGameHours = 12;
        break;
    case EDisasterType::InfectiousDisease:
        DurationInGameHours = 48;
        break;
    default:
        DurationInGameHours = 12;
    }
    

    return CachedTimeManager ? DurationInGameHours * CachedTimeManager->RealSecondsPerGameHour : DurationInGameHours;
}
