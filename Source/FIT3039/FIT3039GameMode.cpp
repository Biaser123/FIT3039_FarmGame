// Copyright Epic Games, Inc. All Rights Reserved.

#include "FIT3039GameMode.h"
#include "FIT3039PlayerController.h"
#include "FIT3039Character.h"
#include "UObject/ConstructorHelpers.h"

AFIT3039GameMode::AFIT3039GameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AFIT3039PlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}