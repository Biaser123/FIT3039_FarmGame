// Fill out your copyright notice in the Description page of Project Settings.


#include "FIT3039/Public/GridActors/Crops.h"

#include "DisasterManager.h"
#include "Cards/Card.h"
#include "Cards/SeedCard.h"
#include "FIT3039/FIT3039PlayerController.h"
#include "GridActors/GridMap.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACrop::ACrop()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CropMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SetRootComponent(CropMesh);

	ProgressionWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("ProgressionWidget"));
	ProgressionWidgetComponent->SetupAttachment(RootComponent);
	ProgressionWidgetComponent->SetVisibility(false);

	WaitingForInteractingIconComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionIconWidget"));
	WaitingForInteractingIconComponent->SetupAttachment(RootComponent);
	WaitingForInteractingIconComponent->SetVisibility(true);

	CropStatusIconComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("StatusBar"));
	CropStatusIconComponent->SetupAttachment(RootComponent);
	CropStatusIconComponent->SetVisibility(true);
	
	Tags.Add(TEXT("Seeding"));
	//SetGrowthStage(EGrowthStage::Seeding);
	SeedingTime = 2.0f;  // default
	WateringTime = 5.0f; // default
	FertilizingTime = 5.0f; //default
	ReapingTime = 3.0f; //default
	
	Price = 0; //default
	
	bCouldMutation = false;

	OxygenValue = 0.f;
	TemperatureValue = 0.f;
	
	bIsInteracting = false;
	bIsInteractable = true;
	bIsBeingInteractedWith = false;

	WaterDecreaseRate = 0.01f;   // total 1

	
	
}

// Called when the game starts or when spawned
void ACrop::BeginPlay()
{
	Super::BeginPlay();

	//get disaster manager
	TArray<AActor*> DisasterManagers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADisasterManager::StaticClass(), DisasterManagers);

	// apply effect 
	if (DisasterManagers.Num() > 0)
	{
		ADisasterManager* DisasterManager = Cast<ADisasterManager>(DisasterManagers[0]); //get the first disaster manager
		if (DisasterManager && DisasterManager->IsDisasterActive())
		{
			ApplyDisasterEffects(DisasterManager->CurrentDisasterType);
		}
	}
}


// Called every frame	
void ACrop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bIsGrowing)
	{
		if (WaterValue > 0)
			{
			WaterValue -= WaterDisasterRateModifier * WaterMutationRateModifier * WaterDecreaseRate * DeltaTime;
			WaterValue = FMath::Clamp(WaterValue, 0.0f, 1.0f); // between 0 -1
			}
		
		    if (WaterValue < 0.4f)
		    {
			    WaterGrowthRateModifier = WaterValue / 0.4f; // growth decrease with value when below 40
		    }
		    else
		    {
			    WaterGrowthRateModifier = 1.0f; // no ratio diff  > 50
		    }

		//  check if needs water
		    if (WaterValue < 0.5f && !Tags.Contains(TEXT("Watering")))
		    {
			    Tags.Add(TEXT("Watering"));
		    	WaterIconEvent.Broadcast();
			    WaitingForInteractingIconComponent ->SetVisibility(true);
		    	
			    bIsInteractable = true;
		    }
		
		    CurrentGrowProgress += (GrowProgressRate * WaterGrowthRateModifier * DiseaseGrowthRateModifier * DisasterGrowthRateModifier) * DeltaTime;
		    if (CurrentGrowProgress >= 1.0f)
		    {
			    CompleteGrowthCycle();
		    }
	}

	if (bCouldMutation)
	{
		UpdateMutation(DeltaTime);
		
		// Check if mutation value reached 1 and apply diseases
		if (MutationValue >= 1.0f)
		{
			ApplyDiseases();
		}
	}

	if(bIsInfected)
	{
		AGridMap* GridMap = Cast<AGridMap>(GetOccupyingNode()->GridMap);
		if (!GridMap) return;
		for (const FGridVector& NeighborCoord : InfectedNeighbors) {
			if (FMath::RandRange(0.0f, 1.0f) < InfectionSpreadChance * DeltaTime) {
				UGridNode* NeighborNode = GridMap->GetNode(NeighborCoord);
				if (NeighborNode) {
					ACrop* NeighborCrop = NeighborNode->GetOccupyingPlant();
					if (NeighborCrop && !NeighborCrop->bIsInfected && NeighborCrop->bIsGrowing) {
						NeighborCrop->bIsInfected = true;
						NeighborCrop->CurrentDiseases.Add(EPlantMutation::PM_InfectiousDisease);
						UMaterialInterface* MutationMaterial = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr, TEXT("/Game/Materials/M_Mutation.M_Mutation")));
						if (MutationMaterial != nullptr)
						{
							NeighborCrop->CropMesh->SetOverlayMaterial(MutationMaterial);
						}
						InfectAffectApplied();
						// Delegate for adding the icons
						NeighborCrop->DiseaseAdded.Broadcast(EPlantMutation::PM_InfectiousDisease);
					}
				}
			}
		}
	}
}

// Called to bind functionality to input
void ACrop::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


void ACrop::StartInteract(AActor* Interactor)
{
	if (!InteractingActors.Contains(Interactor))
	{
		InteractingActors.Add(Interactor);
		if (InteractingActors.Num() == 1)
		{
			bIsInteracting = true;
			WaitingForInteractingIconComponent->SetVisibility(false);
			ProgressionWidgetComponent->SetVisibility(true);

			// Determine the interaction type and set the interaction time accordingly
			if (Tags.Contains(TEXT("Seeding")))
				
			{
				InteractionTime = SeedingTime;
			}
			else if (Tags.Contains(TEXT("Watering")))
			{
				InteractionTime = WateringTime;
			}
			else if (Tags.Contains(TEXT("Reaping")))
			{
				InteractionTime = ReapingTime;
			}
			// Start or continue updating interaction progress every frame
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_Interact, this, &ACrop::UpdateInteractProgress, 1.0f / 60.0f, true);
		}
		// Trigger QTE for player interactions, not relevant for pets
		 if (Tags.Contains(TEXT("Watering")))
		 {
			 AFIT3039PlayerController* PlayerController = Cast<AFIT3039PlayerController>(Interactor);
		 	if (PlayerController)
		 	{
		 		QTEAdded.Broadcast();
		 		PlayerController->ResetQTE();
		 	}
		 }
	}
}



void ACrop::CancelInteract(AActor* Interactor)
{
	if (InteractingActors.Contains(Interactor))
	{
		InteractingActors.Remove(Interactor);
		if (InteractingActors.Num() == 0)
		{
			bIsInteracting = false;
			CurrentProgress = 0.0f;

			// =clear handle
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Interact);
			CropProgressRatio = 0.0f;
			QTERemoved.Broadcast();
			ProgressionWidgetComponent -> SetVisibility(false);
			WaitingForInteractingIconComponent -> SetVisibility(true);
		}
	}
}

void ACrop::CompleteInteract()
{
	if (bIsInteracting)
	{
		if (OnInteractComplete.IsBound())
		{
			UE_LOG(LogTemp, Warning, TEXT("Crop: %s broadcasted OnInteractComplete"), *GetName());
			OnInteractComplete.Execute();
		}
		

		InteractingActors.Empty();
		bIsInteracting = false;
		bIsPetInteracting = false;
		CurrentProgress = 0.0f; // reset progress
		bIsInteractable = false;

		AFIT3039PlayerController* PlayerController = Cast<AFIT3039PlayerController>(UGameplayStatics::GetPlayerController(this, 0));
		if (PlayerController->GetLastInteractedCrop() == this)
		{
			PlayerController->InteractEnd();
		}
		
		// update the tag
		if (Tags.Contains(TEXT("Seeding")))
		{
			Tags.Remove(TEXT("Seeding"));

			if (RowName == FName(TEXT("5")))
			{
				UMaterialInterface* Material = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr, TEXT("/Game/Materials/Plants/M_TorutreeBase.M_TorutreeBase")));
				UAnimationAsset* Anim = Cast<UAnimationAsset>(StaticLoadObject(UAnimationAsset::StaticClass(), nullptr, TEXT("/Game/Animation/AS_Torutree_Phase1.AS_Torutree_Phase1")));
				if (Anim != nullptr)
				{
					CropMesh->PlayAnimation(Anim, true);
					if(Material != nullptr)
						{
						CropMesh->SetMaterial(0, Material);
						
				    }
				}
			}
			else if (RowName == FName(TEXT("1")))
			{
				UMaterialInterface* Material = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr, TEXT("/Game/Materials/Plants/M_IcePepper.M_IcePepper")));
				UAnimationAsset* Anim = Cast<UAnimationAsset>(StaticLoadObject(UAnimationAsset::StaticClass(), nullptr, TEXT("/Game/Animation/AS_IcePepper_Phase1.AS_IcePepper_Phase1")));
				if (Anim != nullptr)
				{
					CropMesh->PlayAnimation(Anim, true);
					if(Material != nullptr)
					{
						CropMesh->SetMaterial(0, Material);
					}
				}
			}
			else if (RowName == FName(TEXT("2")))
			{
				UMaterialInterface* Material = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr, TEXT("/Game/Materials/Plants/M_Cactopus.M_Cactopus")));
				UAnimationAsset* Anim = Cast<UAnimationAsset>(StaticLoadObject(UAnimationAsset::StaticClass(), nullptr, TEXT("/Game/Animation/AS_Cactopus_Phase1.AS_Cactopus_Phase1")));
				if (Anim != nullptr)
				{
					CropMesh->PlayAnimation(Anim, true);
					if(Material != nullptr)
					{
						CropMesh->SetMaterial(0, Material);
					}
				}
			}
			else if (RowName == FName(TEXT("3")))
			{
				UMaterialInterface* Material = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr, TEXT("/Game/Materials/Plants/M_StarPlant.M_StarPlant")));
				UAnimationAsset* Anim = Cast<UAnimationAsset>(StaticLoadObject(UAnimationAsset::StaticClass(), nullptr, TEXT("/Game/Animation/AS_StarPlant_Phase1.AS_StarPlant_Phase1")));
				if (Anim != nullptr)
				{
					CropMesh->PlayAnimation(Anim, true);
					if(Material != nullptr)
					{
						CropMesh->SetMaterial(0, Material);
					}
				}
			}
			Tags.Add(TEXT("Phase1"));
			StartGrowthCycle();
		}
		else if (Tags.Contains(TEXT("Watering")))
		{
			Tags.Remove(TEXT("Watering"));
			//always make the water value >= 80	
			FMath::Min(WaterValue + 0.5f, 1.0f);
			WaterGrowthRateModifier = 1;
			if (WaterValue <= 0.8)
			{
				WaterValue = 0.8;
			}
			
			QTERemoved.Broadcast();
		}
		
		else if (Tags.Contains(TEXT("Reaping")))
		{
			Tags.Remove(TEXT("Reaping"));
			AFIT3039Character* Player = Cast<AFIT3039Character>(UGameplayStatics::GetPlayerCharacter(this, 0));

			if (Player)
			{
				if (RowName == FName(TEXT("2")))
				{
					int increaseAmount = 2 * static_cast<int>(ceil(Price));
					Player->IncreaseGold(increaseAmount);
				}
				else
				{
					int increaseAmount = 3 * static_cast<int>(ceil(Price));
					Player->IncreaseGold(increaseAmount);
				}
				if (RowName == FName(TEXT("1")))
				{
					IcePepperHarvestMaterialEvent.Broadcast();
				}
			}
			
			UGridNode* Node = GetOccupyingNode(); 
			Node->SetOccupant(EOccupantType::Ot_Empty);  //deal the logic for the node
			Destroy();
			
		}
		
		// clear the timer
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Interact);
		
		CropProgressRatio = 0.f;
		ProgressionWidgetComponent -> SetVisibility(false);
		
	}
	
	
}

void ACrop::UpdateInteractProgress()
{
	if (bIsInteracting)
	{
		CurrentProgress += 1.0f / 60.0f * InteractingActors.Num(); 

		float ProgressRatio = CurrentProgress / InteractionTime; 
		ProgressRatio = FMath::Clamp(ProgressRatio, 0.0f, 1.0f);

		this->CropProgressRatio = ProgressRatio;

		if (CurrentProgress >= InteractionTime)
		{
			CompleteInteract();
		}
	}
}

void ACrop::StartGrowthCycle()
{
	if (!bIsGrowing)
	{
		WaitingForInteractingIconComponent -> SetVisibility(false);
		bIsGrowing = true;
		CurrentGrowProgress = 0.0f;
		GrowProgressRate = 1.0f / GrowTime;
	}
}

void ACrop::CompleteGrowthCycle()
{
	CurrentGrowProgress = 0.0f;

	// update the tag
	if (Tags.Contains(TEXT("Phase1")))
	{
		Tags.Remove(TEXT("Phase1"));
		Tags.Add(TEXT("Phase2"));
		if (RowName == FName(TEXT("5"))) //5 is toru tree
		{
			USkeletalMesh* Mesh = Cast<USkeletalMesh>(StaticLoadObject(USkeletalMesh::StaticClass(), nullptr, TEXT("/Game/Models/Plants/SM_Torutree_Phase2.SM_Torutree_Phase2")));
			UAnimSequence* AnimationPhase2 = Cast<UAnimSequence>(StaticLoadObject(UAnimSequence::StaticClass(), nullptr, TEXT("/Game/Animation/AS_Torutree_Phase2.AS_Torutree_Phase2")));

			if (Mesh != nullptr)
			{
				CropMesh->SetSkeletalMesh(Mesh);
				if (AnimationPhase2 != nullptr)
				{
					CropMesh->PlayAnimation(AnimationPhase2, true);
				}
			}
		}
		else if (RowName == FName(TEXT("1")))  // 1 is ice pepper
		{
			USkeletalMesh* Mesh = Cast<USkeletalMesh>(StaticLoadObject(USkeletalMesh::StaticClass(), nullptr, TEXT("/Game/Models/Plants/SM_IcePepper_Phase2.SM_IcePepper_Phase2")));
			UAnimSequence* AnimationPhase2 = Cast<UAnimSequence>(StaticLoadObject(UAnimSequence::StaticClass(), nullptr, TEXT("/Game/Animation/AS_IcePepper_Phase2.AS_IcePepper_Phase2")));

			if (Mesh != nullptr)
			{
				CropMesh->SetSkeletalMesh(Mesh);
				if (AnimationPhase2 != nullptr)
				{
					CropMesh->PlayAnimation(AnimationPhase2, true);
				}
			}
		}
		else if (RowName == FName(TEXT("2")))  // 2 is cactopus
		{
			USkeletalMesh* Mesh = Cast<USkeletalMesh>(StaticLoadObject(USkeletalMesh::StaticClass(), nullptr, TEXT("/Game/Models/Plants/SM_Cactopus_Phase2.SM_Cactopus_Phase2")));
			UAnimSequence* AnimationPhase2 = Cast<UAnimSequence>(StaticLoadObject(UAnimSequence::StaticClass(), nullptr, TEXT("/Game/Animation/AS_Cactopus_Phase2.AS_Cactopus_Phase2")));

			if (Mesh != nullptr)
			{
				CropMesh->SetSkeletalMesh(Mesh);
				if (AnimationPhase2 != nullptr)
				{
					CropMesh->PlayAnimation(AnimationPhase2, true);
				}
			}
		}
		else if (RowName == FName(TEXT("3")))  // 3 is starPlant
		{
			UMaterialInterface* Material = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr, TEXT("/Game/Materials/Plants/M_StarPlant_Phase2.M_StarPlant_Phase2")));
			USkeletalMesh* Mesh = Cast<USkeletalMesh>(StaticLoadObject(USkeletalMesh::StaticClass(), nullptr, TEXT("/Game/Models/Plants/SM_StarPlant_Phase2.SM_StarPlant_Phase2")));
			UAnimSequence* AnimationPhase2 = Cast<UAnimSequence>(StaticLoadObject(UAnimSequence::StaticClass(), nullptr, TEXT("/Game/Animation/AS_StarPlant_Phase2.AS_StarPlant_Phase2")));

			if (Mesh != nullptr)
			{
				CropMesh->SetSkeletalMesh(Mesh);
				if (AnimationPhase2 != nullptr)
				{
					CropMesh->PlayAnimation(AnimationPhase2, true);
				}
				if(Material != nullptr)
				{
					CropMesh->SetMaterial(0,Material);
					CropMesh->SetMaterial(1,Material);
				}
			}
		}
	}
	else if (Tags.Contains(TEXT("Phase2")))
	{
		Tags.Remove(TEXT("Phase2"));
		Tags.Add(TEXT("Phase3"));
		GrowProgressRate *= 0.5;
		bCouldMutation = true;

		if (RowName == FName(TEXT("5")))
		{
			USkeletalMesh* Mesh = Cast<USkeletalMesh>(StaticLoadObject(USkeletalMesh::StaticClass(), nullptr, TEXT("/Game/Models/Plants/SM_Torutree_Phase3.SM_Torutree_Phase3")));
			UAnimSequence* AnimationPhase3 = Cast<UAnimSequence>(StaticLoadObject(UAnimSequence::StaticClass(), nullptr, TEXT("/Game/Animation/AS_Torutree_Phase3.AS_Torutree_Phase3")));

			if (Mesh != nullptr)
			{
				CropMesh->SetSkeletalMesh(Mesh);
				if (AnimationPhase3 != nullptr)
				{
					CropMesh->PlayAnimation(AnimationPhase3, true);
				}
			}
		}
		else if (RowName == FName(TEXT("1")))  // 1 is ice pepper
		{
			USkeletalMesh* Mesh = Cast<USkeletalMesh>(StaticLoadObject(USkeletalMesh::StaticClass(), nullptr, TEXT("/Game/Models/Plants/SM_IcePepper_Phase3.SM_IcePepper_Phase3")));
			UAnimSequence* AnimationPhase2 = Cast<UAnimSequence>(StaticLoadObject(UAnimSequence::StaticClass(), nullptr, TEXT("/Game/Animation/AS_IcePepper_Phase3.AS_IcePepper_Phase3")));

			if (Mesh != nullptr)
			{
				CropMesh->SetSkeletalMesh(Mesh);
				CropMesh->SetWorldScale3D(FVector(1.2, 1.2, 1.2));
				if (AnimationPhase2 != nullptr)
				{
					CropMesh->PlayAnimation(AnimationPhase2, true);
				}
			}
		}
		else if (RowName == FName(TEXT("2")))  // 2 is cactopus
		{
			USkeletalMesh* Mesh = Cast<USkeletalMesh>(StaticLoadObject(USkeletalMesh::StaticClass(), nullptr, TEXT("/Game/Models/Plants/SM_Cactopus_Phase3.SM_Cactopus_Phase3")));
			UAnimSequence* AnimationPhase2 = Cast<UAnimSequence>(StaticLoadObject(UAnimSequence::StaticClass(), nullptr, TEXT("/Game/Animation/AS_Cactopus_Phase3.AS_Cactopus_Phase3")));

			if (Mesh != nullptr)
			{
				CropMesh->SetSkeletalMesh(Mesh);
				if (AnimationPhase2 != nullptr)
				{
					CropMesh->PlayAnimation(AnimationPhase2, true);
				}
			}
		}
		else if (RowName == FName(TEXT("3")))  // 3 is starPlant
		{
			UMaterialInterface* Material = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr, TEXT("/Game/Materials/Plants/M_StarPlant_Phase3.M_StarPlant_Phase3")));
			USkeletalMesh* Mesh = Cast<USkeletalMesh>(StaticLoadObject(USkeletalMesh::StaticClass(), nullptr, TEXT("/Game/Models/Plants/SM_StarPlant_Phase3.SM_StarPlant_Phase3")));
			UAnimSequence* AnimationPhase2 = Cast<UAnimSequence>(StaticLoadObject(UAnimSequence::StaticClass(), nullptr, TEXT("/Game/Animation/AS_StarPlant_Phase3.AS_StarPlant_Phase3")));

			if (Mesh != nullptr)
			{
				CropMesh->SetSkeletalMesh(Mesh);
				CropMesh->SetWorldScale3D(FVector(1.2, 1.2, 1.2));
				if (AnimationPhase2 != nullptr)
				{
					CropMesh->PlayAnimation(AnimationPhase2, true);
				}
				if(Material != nullptr)
				{
					
					CropMesh->SetMaterial(0, Material);
					CropMesh->SetMaterial(1, Material);
				}
			}
		}
	}
	else if (Tags.Contains(TEXT("Phase3")))
	{
		Tags.Remove(TEXT("Phase3"));
		//execute the pet if have 
		if (OnInteractComplete.IsBound())
		{
			UE_LOG(LogTemp, Warning, TEXT("Crop: %s broadcasted OnInteractComplete"), *GetName());
			OnInteractComplete.Execute();
		}
		InteractingActors.Empty();
		Tags.Remove(TEXT("Watering"));
		bIsInteractable = false;
		Tags.Add(TEXT("Reaping"));
		bCouldMutation = false; 
		bIsGrowing = false;
		QTERemoved.Broadcast();
		ProgressionWidgetComponent->SetVisibility(false);
		//bIsInteracting = false;
		CurrentProgress = 0.0f;
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Interact);
		CropProgressRatio = 0.f;

		HarvestingIconEvent.Broadcast();
		WaitingForInteractingIconComponent ->SetVisibility(true);
		bIsInteractable = true;
	}
}

UGridNode* ACrop::GetOccupyingNode() const
{
	return OccupyNode;
}

void ACrop::SetOccupyingNode(UGridNode *Node)
{
	OccupyNode = Node;
}

void ACrop::UpdateMutation(float DeltaTime)
{
	MutationValue += MutationRate * DeltaTime;
}

// apply the mutation (has a prob)
void ACrop::ApplyDiseases()
{
	for (const FDiseaseProbability& Disease : MutationProbabilities)
	{
		if (FMath::RandRange(0.0f, 1.0f) < Disease.Probability && !HasDisease(Disease.MutationType))
		{
			// Apply the disease effects based on the type
			switch (Disease.MutationType)
			{
			case EPlantMutation::PM_BlackRot:
				// Causes root decay and reduced growth
				DiseaseGrowthRateModifier  *= 0.5f;  // Example modifier, adjust as needed
				break;
			case EPlantMutation::PM_LeafScorch:
				// Leaves dry out will reduced growth, but also decrease the water needed
				DiseaseGrowthRateModifier  *= 0.75f; // Reduced growth rate
				WaterMutationRateModifier *= 0.8f;  // Needs less water
				break;
			case EPlantMutation::PM_FruitWilt:	
				// Fruits develop improperly reducing yield, but increase growth rate
				Price *= 0.7f;  // Reduced yield
				DiseaseGrowthRateModifier  *= 1.2f; // Increased growth rate
				break;
			case EPlantMutation::PM_InfectiousDisease:
				bIsInfected = true;
				InfectAffectApplied();
				break;
			default:
				break;
			}
			
			
			// Add to a list of current diseases if not already present
			
			CurrentDiseases.Add(Disease.MutationType);
			
			// Delegate for adding the icons and particle effect
			DiseaseAdded.Broadcast(Disease.MutationType);
			
			UMaterialInterface* MutationMaterial = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr, TEXT("/Game/Materials/M_Mutation.M_Mutation")));
			if (MutationMaterial != nullptr)
			{
				CropMesh->SetOverlayMaterial(MutationMaterial);
			}
			MutationValue = 0.0f;
		}
	}
}

void ACrop::ClearDiseases()
{
	// Reset all modifiers to their default values
	DiseaseGrowthRateModifier = 1.0f;  // Reset growth rate modifier to no change
	WaterMutationRateModifier = 1.0f;       // Reset water decrease rate to normal
	Price = OriginalPrice;    // Reset price to default
	bIsInfected = false;    //reset the infect
	// Clear the list of current diseases
	CurrentDiseases.Empty();
	//reset
	CropMesh->SetOverlayMaterial(nullptr);
	DiseaseRemoved.Broadcast();
	MutationValue = 0.f;
	// no mutation anymore
	MutationRate = 0.f;
}


bool ACrop::HasDisease(EPlantMutation DiseaseType) const
{
	return CurrentDiseases.Contains(DiseaseType);
}

void ACrop::GetNeighborsNodes()
{
	//following is to get the nearby node 
	//get the node first
	UGridNode* MyNode = GetOccupyingNode();
	if (!MyNode)
	{
		UE_LOG(LogTemp, Warning, TEXT("Node is NULL"));
		return;
	}

	//get the map
	AGridMap* GridMap = Cast<AGridMap>(MyNode->GridMap);
	if(!GridMap)
	{
		UE_LOG(LogTemp, Warning, TEXT("GridMap is NULL"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("find gridmap"));
	FGridVector MyCoord = MyNode->Coordinate;

	// get the nearby node
	TArray<FGridVector> Directions = {
		FGridVector(-1, -1), FGridVector(-1, 0), FGridVector(-1, 1),
		FGridVector(0, -1),                             FGridVector(0, 1),
		FGridVector(1, -1), FGridVector(1, 0), FGridVector(1, 1)
	};

	for (const FGridVector& Dir : Directions) {
		FGridVector NeighborCoord = MyCoord + Dir;
		// check if the node is valid
		if (GridMap->IsCoordValid(NeighborCoord))
		{
			InfectedNeighbors.Add(NeighborCoord);
		}
	}
}

int ACrop::GetMutationCount()
{
	return CurrentDiseases.Num();
}

// void ACrop::SetGrowthStage(EGrowthStage NewGrowthStage)
// {
// 	GrowthStage = NewGrowthStage;
// }

void ACrop::ApplyDisasterEffects(EDisasterType DisasterType)
{
	switch (DisasterType)
	{
	case EDisasterType::Drought:
		WaterDisasterRateModifier *= 2; 
		break;
	case EDisasterType::ColdSnap:
		DisasterGrowthRateModifier *= 1.2; 
		break;
	case EDisasterType::InfectiousDisease:
		FDiseaseProbability InfectiousDisease;
		InfectiousDisease.MutationType = EPlantMutation::PM_InfectiousDisease;
		InfectiousDisease.Probability = 0.3f;
		MutationProbabilities.Add(InfectiousDisease);
		break;
	default:
		break; // No effect if disaster type is not recognized
	}
}

void ACrop::InfectAffectApplied()
{
	DisasterGrowthRateModifier *= 1.5;
	Price *= 0.5;
}
