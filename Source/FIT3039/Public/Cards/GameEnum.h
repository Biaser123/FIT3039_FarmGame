// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


UENUM(BlueprintType)
enum class EDisasterType : uint8
{
	ColdSnap UMETA(DisplayName = "Cold Snap"),
	Drought UMETA(DisplayName = "Drought"),
	InfectiousDisease UMETA(DisplayName = "Infectious Disease"),
	Default UMETA(DisplayName = "Default")
};

UENUM(BlueprintType)
enum class EPlantMutation : uint8
{
	PM_None UMETA(DisplayName = "None", Tooltip = "No disease has affected the plant. 没有疾病影响植物"),
	PM_BlackRot UMETA(DisplayName = "Black Rot", Tooltip = "reduced growth 生长速度下降"),
	PM_LeafScorch UMETA(DisplayName = "Leaf Scorch", Tooltip = "减少生长速度,但会减少浇水的需求"),
	PM_FruitWilt UMETA(DisplayName = "Fruit Wilt", Tooltip = "reducing yield, but increase growth. 产量降低，但会增长生长速度"),
	PM_InfectiousDisease UMETA(DisplayName = "Infectiouse Disease", Tooltip = "infect nearby crops and reduce yield")
};

UENUM(BlueprintType)
enum class EDrawCardResult: uint8
{
	NewCard,
	IncreasedQuantity,
	NotFound
};

UENUM(BlueprintType)
enum class EUseCardResult: uint8
{
	OnlyOneLast,
	DecreaseQuantity,
	NoCards
};