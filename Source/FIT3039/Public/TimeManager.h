// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimeManager.generated.h"
// day-night change delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTimeEvent);

UCLASS()
class FIT3039_API ATimeManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATimeManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Current hour in game time
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Time")
	float CurrentHour = 8.0f;

	// Total number of full days elapsed in game
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Time")
	int TotalDays = 1;

	// Number of real-world seconds for each game hour to pass
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Time")
	float RealSecondsPerGameHour = 25.0f;

	// Number of hours during daylight in game time
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Time")
	int DayTimeHours = 14;

	void OnDayNightChange();
	
	
	UPROPERTY(BlueprintAssignable, Category="Time Events")
	FOnTimeEvent OnNightStarted;

	UPROPERTY(BlueprintAssignable, Category="Time Events")
	FOnTimeEvent OnDayStarted;

	UPROPERTY(BlueprintAssignable, Category="Time Events")
	FOnTimeEvent OnDayChanged;


private:
	
	
	// Accumulator for time to handle incrementing game hours
	float AccumulatedTime = 0.0f;

	bool bIsNight = false;
	
	
};
