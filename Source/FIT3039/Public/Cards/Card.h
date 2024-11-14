// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Card.generated.h"

UENUM(BlueprintType)
enum class ECardType : uint8
{
	Seed UMETA(DisplayName = "Seed"),
	Potion UMETA(DisplayName = "Potion"),
	Pet UMETA(DisplayName = "Pet"),
	Fuel UMETA(DisplayName = "Fuel")
};

UCLASS()
class FIT3039_API ACard : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACard();
	
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Card Type")
	ECardType CardType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Name")
	FString CardName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Description")
	FString CardDescription;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int Price;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int NumOfSameCard;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};