// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeManager.h"


// Sets default values
ATimeManager::ATimeManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}


// Called when the game starts or when spawned
void ATimeManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATimeManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Accumulate the time elapsed
	AccumulatedTime += DeltaTime;
	// Check if enough real time has passed to increment one game hour
	if (AccumulatedTime >= RealSecondsPerGameHour)
	{
		// Increment the game hour
		CurrentHour += 1.0f;
		// Reset the time accumulator
		AccumulatedTime -= RealSecondsPerGameHour;

		// Check if a full day has passed
		if (CurrentHour >= 24.0f)
		{
			// Reset the hour count and increment the day count
			CurrentHour -= 24.0f;
			TotalDays += 1;
			OnDayChanged.Broadcast(); 
		}
	}

	// Determine if it is night or day
	bool newIsNight = (CurrentHour >= 20.0f || CurrentHour < 8.0f);
	if (newIsNight != bIsNight)
	{
		bIsNight = newIsNight;
		OnDayNightChange();
		
	}
}

void ATimeManager::OnDayNightChange()
{
	if (bIsNight)
	{
		// Notify all listeners that it's now night
		OnNightStarted.Broadcast();
	}
	else
	{
		// Notify all listeners that it's now day
		OnDayStarted.Broadcast();
	}
}

