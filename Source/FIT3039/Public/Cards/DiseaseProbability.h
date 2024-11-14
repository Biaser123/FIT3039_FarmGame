// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#pragma once

#include "CoreMinimal.h"
#include "FIT3039/Public/Cards/GameEnum.h" 
#include "DiseaseProbability.generated.h"

USTRUCT(BlueprintType)
struct FDiseaseProbability
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPlantMutation MutationType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Probability; 
};
