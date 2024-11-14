// Fill out your copyright notice in the Description page of Project Settings.


#include "GridActors/PetCharacter.h"

// Sets default values
APetCharacter::APetCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 5, 0);

	Price = 0.f;
	OxygenValue = 1.f;

}

// Called when the game starts or when spawned
void APetCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APetCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APetCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

