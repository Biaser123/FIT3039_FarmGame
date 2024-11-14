// Fill out your copyright notice in the Description page of Project Settings.


#include "GridActors/PetAIController.h"

#include "GridActors/LockTheCropTask.h"
#include "GridActors/PetCharacter.h"


APetAIController::APetAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	SightConfiguration = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Configuration"));
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component")));

	SightConfiguration->SightRadius = SightRadius;
	SightConfiguration->LoseSightRadius = LoseSightRadius;
	SightConfiguration->PeripheralVisionAngleDegrees = FieldOfView;
	SightConfiguration->SetMaxAge(SightAge);
	SightConfiguration->DetectionByAffiliation.bDetectEnemies = true;
	SightConfiguration->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfiguration->DetectionByAffiliation.bDetectNeutrals = true;

	GetPerceptionComponent()->SetDominantSense(*SightConfiguration->GetSenseImplementation());
	GetPerceptionComponent()->ConfigureSense(*SightConfiguration);
	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &APetAIController::OnSensesUpdated);
	TargetCrop = nullptr;
}

void APetAIController::BeginPlay()
{
	Super::BeginPlay();

	if(!AIBlackboard)
		return;
	if(!ensure(BehaviorTree))
		return;
	UseBlackboard(AIBlackboard, BlackboardComponent);
	RunBehaviorTree(BehaviorTree);

	NavigationSystem = Cast<UNavigationSystemV1>(GetWorld()->GetNavigationSystem());
	GenerateNewRandomLocation();
	
}

void APetAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void APetAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	for (int32 i = SeenCrops.Num() - 1; i >= 0; --i)
	{
		ACrop* CropActor = SeenCrops[i];
		if (CropActor && CropActor->bIsInteractable && CropActor->ActorHasTag(RequiredTag))
		{
			DetectedCrops.AddUnique(CropActor);
		}
	}
	for (int32 i = DetectedCrops.Num() - 1; i >= 0; --i)
	{
		ACrop* CropActor = DetectedCrops[i];
		if (!CropActor || !CropActor->bIsInteractable || !CropActor->ActorHasTag(RequiredTag))
		{
			DetectedCrops.RemoveAt(i);
		}
	}
	UpdateNearestCrop();
}

FRotator APetAIController::GetControlRotation() const
{
	if(GetPawn())
	{
		return FRotator(0, GetPawn()->GetActorRotation().Yaw, 0);
	}
	return FRotator::ZeroRotator;
}


void APetAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	GenerateNewRandomLocation();
}



void APetAIController::GenerateNewRandomLocation()
{
	if (!NavigationSystem)
	{
		UE_LOG(LogTemp, Error, TEXT("NavigationSystem is null."));
		return;
	}

	FVector CurrentLocation = GetPawn() ? GetPawn()->GetActorLocation() : FVector::ZeroVector;
	FNavLocation NavPoint;

	// Try to project the current location onto the nearest navigable point within a large area.
	if (!NavigationSystem->ProjectPointToNavigation(CurrentLocation, NavPoint, FVector(5000, 5000, 5000))) // The FVector(5000, 5000, 5000) is a broad search extent.
	{
		// If the current location is not on the navigation mesh, find a random point on the navigation mesh.
		UE_LOG(LogTemp, Warning, TEXT("Current location is not on the navigation mesh. Finding an accessible point on the mesh."));
		if (!NavigationSystem->GetRandomPoint(NavPoint))
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to find an accessible point on the navigation mesh."));
			return;
		}
	}
	else
	{
		// If already on the navigation mesh, find another random navigable point from the current position.
		if (!NavigationSystem->GetRandomPointInNavigableRadius(CurrentLocation, 1000, NavPoint))
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to find a new random navigable point from the current location."));
			return;
		}
	}
	

	// Set the patrol point to the newly found navigable location.
	BlackboardComponent->SetValueAsVector("PatrolPoint", NavPoint.Location);
}



void APetAIController::OnSensesUpdated(AActor* UpdatedActor, FAIStimulus Stimulus)
{
	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>() && Stimulus.WasSuccessfullySensed())
	{
		ACrop* CropActor = Cast<ACrop>(UpdatedActor);
		// when 
		if (CropActor && !CropActor->bIsPetInteracting)
		{
			SeenCrops.AddUnique(CropActor);
		}
	}
	else
	{
		ACrop* CropActor = Cast<ACrop>(UpdatedActor);
		if (CropActor)
		{
			SeenCrops.Remove(CropActor);
		}
	}
}


void APetAIController::UpdateNearestCrop()
{
	if(TargetCrop)
	{
		return;
	}
	
	ACrop* NearestCrop = nullptr;
	float NearestDistanceSq = FLT_MAX;
	FVector MyLocation = GetPawn()->GetActorLocation();
	
	for (ACrop* CropActor : DetectedCrops)
	{
		if (CropActor && !CropActor->bIsPetInteracting && CropActor->bIsInteractable && CropActor->ActorHasTag(RequiredTag))
		{
			float DistanceSq = FVector::DistSquared(MyLocation, CropActor->GetActorLocation());
			if (DistanceSq < NearestDistanceSq)
			{
				NearestDistanceSq = DistanceSq;
				NearestCrop = CropActor;
			}
		}
	}
		
	if (NearestCrop)
	{
		BlackboardComponent->SetValueAsBool("FoundCrop", true);
		BlackboardComponent->SetValueAsVector("CropLocation", NearestCrop->GetActorLocation());
		BlackboardComponent->SetValueAsObject("TargetCrop", NearestCrop);
		TargetCrop = NearestCrop;
	}
	else
	{
		BlackboardComponent->SetValueAsBool("FoundCrop", false);
		BlackboardComponent->ClearValue("TargetCrop");
	}
}

void APetAIController::RemoveCropFromDetected(ACrop* CropActor)
{
	if (CropActor)
	{
		DetectedCrops.RemoveSingle(CropActor);
	}
}
