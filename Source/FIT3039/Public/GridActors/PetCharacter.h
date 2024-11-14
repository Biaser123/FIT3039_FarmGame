// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PetCharacter.generated.h"


UENUM(BlueprintType)
enum class EPetSkills : uint8
{
	Seeding UMETA(DisplayName = "Seeding"),
	Watering UMETA(DisplayName = "Watering"),
	Harvesting UMETA(DisplayName = "Harvesting"),
	Default UMETA(DisplayName = "Default")
};

UCLASS()
class FIT3039_API APetCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APetCharacter();

protected:	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pet")
	float OxygenValue;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int Price;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EPetSkills PetSkills;
};
