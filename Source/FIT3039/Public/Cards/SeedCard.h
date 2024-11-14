// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Card.h"
#include "DiseaseProbability.h"
#include "SeedCard.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EGrowthStage : uint8
{
	Seeding,
	FirstStage,
	Watering,
	SecondStage,
	Fertilizing,
	ThirdStage,
	Raping
};


UCLASS()
class FIT3039_API ASeedCard : public ACard
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float SeedingTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float WateringTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float FertilizeTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float GrowTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Disease")
	TArray<FDiseaseProbability> DiseasesMayGet;
	
};
