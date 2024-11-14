// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataTable.h"
#include "FIT3039/Public/Cards/DiseaseProbability.h"
#include "CardDataStruct.generated.h"

enum class ECardType : uint8;
/**
 * 
 */
USTRUCT(BlueprintType)
struct FCardData : public FTableRowBase
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="RowName"))
	FName RowName;
	
	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="CardName",MakeStructureDefaultValue="Card Name"))
	FString CardName;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="CardType", MakeStructureDefaultValue="Seed"))
	ECardType CardType;
	
	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="CardDescription", MakeStructureDefaultValue="Card Description"))
	FString CardDescription;
	
	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Card_Image", MakeStructureDefaultValue="None"))
	TObjectPtr<UTexture2D> Card_Image;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Card_Background", MakeStructureDefaultValue="None"))
	TObjectPtr<UTexture2D> Card_Background;

	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="CardPrice", MakeStructureDefaultValue="2"))
	int CardPrice;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="CardNumber", MakeStructureDefaultValue="1"))
	int CardNum;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="GrowTime", MakeStructureDefaultValue="1"))
	float GrowTime;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="DiseasesMayGet"))
	TArray<FDiseaseProbability> DiseasesMayGet;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="OxygenValue", MakeStructureDefaultValue="1"))
	float OxygenValue;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="TemperatureValue", MakeStructureDefaultValue="1"))
	float TemperatureValue;
	
	
	
	bool operator==(const FCardData& Other) const
	{
		return CardName == Other.CardName; 
	}
};


