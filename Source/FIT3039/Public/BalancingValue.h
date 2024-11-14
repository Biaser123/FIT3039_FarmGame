// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EngineUtils.h"
#include "TimeManager.h"

#include "BalancingValue.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGameOverDelegate);

UENUM(BlueprintType)
enum class ETemperatureStatus : uint8
{
	Normal UMETA(DisplayName = "Normal"),
	Affected UMETA(DisplayName = "Affected"),
	Deadly UMETA(DisplayName = "Deadly")
};

UENUM(BlueprintType)
enum class EOxygenLevelStatus : uint8
{
	Normal UMETA(DisplayName = "Normal"),
	Affected UMETA(DisplayName = "Affected"),
	Deadly UMETA(DisplayName = "Deadly")
};

UCLASS()
class FIT3039_API ABalancingValue : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABalancingValue();

	// Functions to update and get the environment values
	UFUNCTION(BlueprintCallable, Category = "Environment")
	void UpdateEnvironmentValues();

	UFUNCTION(BlueprintCallable, Category = "Environment")
	ETemperatureStatus GetTemperatureStatus() const;

	UFUNCTION(BlueprintCallable, Category = "Environment")
	EOxygenLevelStatus GetOxygenLevelStatus() const;

	// Function to simulate plant's effect on environment
	UFUNCTION(BlueprintCallable, Category = "Environment")
	void UpdateEnvironment();

	//oxygen decrease when night
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	float DailyOxygenDecrease;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	float NightTemperatureDrop;

	UFUNCTION()
	void HandleNightStarted();
	UFUNCTION()
	void HandleDayStarted();
	UFUNCTION()
	void HandleDayChanged();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	float BaseTemperature;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	float BaseOxygenLevel;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FGameOverDelegate FGameOver;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FGameOverDelegate FDangerWarning;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Environment")
	float CurrentTemperature;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	float CropTemperature;

	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	float CropOxygenLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	float PetOxygenLevel;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Environment")
	float CurrentOxygenLevel;

private:
	ATimeManager* TimeManager;

	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
};
