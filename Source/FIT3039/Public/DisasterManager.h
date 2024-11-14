// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BalancingValue.h"
#include "TimeManager.h"
#include "GameFramework/Actor.h"
#include "FIT3039/Public/Cards/GameEnum.h"
#include "DisasterManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDisasterEvent, EDisasterType, DisasterType);



UCLASS()
class FIT3039_API ADisasterManager : public AActor
{
	GENERATED_BODY()

public:
	ADisasterManager();
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void CheckForDisaster();
	void TriggerDisaster();

	void ApplyDisasterEffects(EDisasterType Disaster);
	void ClearDisasterEffects(EDisasterType Disaster);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Disaster")
	float DisasterChance = 0.1f;  

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Disaster")
	TArray<float> DisasterDurations;



public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Disaster")
	EDisasterType CurrentDisasterType = EDisasterType::Default;
	
	UPROPERTY(BlueprintAssignable, Category="Disaster Events")
	FOnDisasterEvent OnDisasterWarning;
	
	FOnDisasterEvent OnDisasterStarted;
	
	UPROPERTY(BlueprintAssignable, Category="Disaster Events")
	FOnDisasterEvent OnDisasterEnded;

	bool IsDisasterActive() const { return bIsDisasterActive; }

	float GetDisasterDuration(EDisasterType DisasterType);



private:

	float TimeSinceLastCheck = 0.0f;
	bool bIsDisasterActive = false;
	bool bIsDisasterHappening = false;
	

	TArray<float> OriginalWaterDecreaseRates;

	ATimeManager* CachedTimeManager;
	ABalancingValue* CachedBalancingValue;
};
