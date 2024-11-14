// Fill out your copyright notice in the Description page of Project Settings.


#include "ChargingStation.h"
#include "Math/UnrealMathUtility.h"
#include "Engine/World.h"

#include "FIT3039/FIT3039PlayerController.h"



// Sets default values
AChargingStation::AChargingStation()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	StationMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StationMesh"));
	FuelTankMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FuelTankMesh"));
	FuelTankSignMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FuelTankSignMesh"));

	SetRootComponent(StationMesh);
	FuelTankMesh->SetupAttachment(StationMesh);
	FuelTankSignMesh->SetupAttachment(StationMesh);

	DetectionSphere = CreateDefaultSubobject<UBoxComponent>(TEXT("DetectionSphere"));
	DetectionSphere->SetupAttachment(RootComponent);
	DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AChargingStation::OnOverlapBegin);
	DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &AChargingStation::OnOverlapEnd);

	//Interact UI Tutor
	InteractUIComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractTip"));
	InteractUIComponent->SetupAttachment(RootComponent);
	InteractUIComponent->SetVisibility(false);
	//progression progress UI for repairing
	ProgressionWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("ProgressionWidget"));
	ProgressionWidgetComponent->SetupAttachment(RootComponent);
	ProgressionWidgetComponent->SetVisibility(false);

	InteractionTime = 10.f;
	ChargingCount = 0;
	CurrentProgress = 0.f;

	Stage1Mesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, TEXT("/Game/Models/FuelTankSIgn/FuelTankSignStage1.FuelTankSignStage1")));
	Stage2Mesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, TEXT("/Game/Models/FuelTankSIgn/FuelTankSignStage2.FuelTankSignStage2")));
	Stage3Mesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, TEXT("/Game/Models/FuelTankSIgn/FuelTankSignStage3.FuelTankSignStage3")));
	Stage4Mesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, TEXT("/Game/Models/FuelTankSIgn/FuelTankSignStage4.FuelTankSignStage4")));
	Stage5Mesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, TEXT("/Game/Models/FuelTankSIgn/FuelTankSignStage5.FuelTankSignStage5")));
	Stage6Mesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, TEXT("/Game/Models/FuelTankSIgn/FuelTankSignStage6.FuelTankSignStage6")));
	Stage7Mesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, TEXT("/Game/Models/FuelTankSIgn/FuelTankSignStage7.FuelTankSignStage7")));
	Stage8Mesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, TEXT("/Game/Models/FuelTankSIgn/FuelTankSignStageFull.FuelTankSignStageFull")));
}



void AChargingStation::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AFIT3039Character* PlayerCharacter = Cast<AFIT3039Character>(OtherActor);
	if (PlayerCharacter)
	{
		PlayerCharacter->SetIsOverlappingWithChargingStation(true);
		if(bLoadedFuelTank && !bIsCharging)
		{
			InteractUIComponent->SetVisibility(true);
		}
	}
}

void AChargingStation::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AFIT3039Character* PlayerCharacter = Cast<AFIT3039Character>(OtherActor);
	if (PlayerCharacter)
	{
		PlayerCharacter->SetIsOverlappingWithChargingStation(false);
		InteractUIComponent->SetVisibility(false); 
	}
}

void AChargingStation::StartInteract()
{
	bIsCharging = true;
	ProgressionWidgetComponent->SetVisibility(true);
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_Interact, this, &AChargingStation::UpdateInteractProgress, 1.0f / 60.0f, true);
	UE_LOG(LogTemp, Warning, TEXT("Timer set for UpdateInteractProgress"));
	QTEAddedEvent.Broadcast();
	//Trigger QTE for player interactions
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	
	if (PlayerController)
	{
		AFIT3039PlayerController* FIT3039PlayerController = Cast<AFIT3039PlayerController>(PlayerController);
		FIT3039PlayerController->ResetQTE();
	}
	// Start a timer to check QTE every second
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_CheckQTE, this, &AChargingStation::CheckQTE, 1.0f, true);
}
void AChargingStation::CancelInteract()
{
	ShutInteract();
	QTERemovedEvent.Broadcast();
	
}

void AChargingStation::ShutInteract()
{
	bIsCharging = false;
	CurrentProgress = 0.f;

	// =clear handle
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Interact);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_CheckQTE);
	ChargingProgressRatio = 0.f;
	ProgressionWidgetComponent-> SetVisibility(false);
	if(ChargingStationInteractingMaterial)
	{
		StationMesh->SetMaterial(1,ChargingStationNotInteractingMaterial);
	}
	
}

void AChargingStation::CompleteInteract()
{
	if(bIsCharging)
	{
		bIsCharging = false;
		ChargingProgressRatio = 0.f;
		ProgressionWidgetComponent -> SetVisibility(false);
		QTERemovedEvent.Broadcast();
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Interact);
		FuelTankUsed();
		bIsInteractable = false;
		if(ChargingStationInteractingMaterial)
		{
			StationMesh->SetMaterial(1,ChargingStationNotInteractingMaterial);
		}

		ChargingCount += 1;
		
		switch (ChargingCount)
		{
		case 1:
			FuelTankSignMesh->SetStaticMesh(Stage1Mesh);
			break;
		case 2:
			FuelTankSignMesh->SetStaticMesh(Stage2Mesh);
			break;
		case 3:
			FuelTankSignMesh->SetStaticMesh(Stage3Mesh);
			break;
		case 4:
			FuelTankSignMesh->SetStaticMesh(Stage4Mesh);
			break;
		case 5:
			FuelTankSignMesh->SetStaticMesh(Stage5Mesh);
			break;
		case 6:
			FuelTankSignMesh->SetStaticMesh(Stage6Mesh);
			break;
		case 7:
			FuelTankSignMesh->SetStaticMesh(Stage7Mesh);
			break;
		case 8:
			FuelTankSignMesh->SetStaticMesh(Stage8Mesh);
			FGameWin.Broadcast();
			break;
		
		default:
			break;
		} 
	
	}
	
}


void AChargingStation::UpdateInteractProgress()
{
	if (bIsCharging)
	{
		CurrentProgress += 1.0f / 60.0f; 
		
		float ProgressRatio = CurrentProgress / InteractionTime; 
		ProgressRatio = FMath::Clamp(ProgressRatio, 0.0f, 1.0f);

		this->ChargingProgressRatio = ProgressRatio;

		if (CurrentProgress >= InteractionTime)
		{
			CompleteInteract();
		}
		
	}
	
}

// Called when the game starts or when spawned
void AChargingStation::BeginPlay()
{
	Super::BeginPlay();
	FuelTankMesh->SetVisibility(false);

	if (FuelTankMesh && FuelTankTransparentMaterial)
	{
		DynamicTransparentMaterial = UMaterialInstanceDynamic::Create(FuelTankTransparentMaterial, this);
		FuelTankMesh->SetMaterial(0, DynamicTransparentMaterial);
	}

	if (FuelTankMesh)
	{
		InitialZ = FuelTankMesh->GetComponentLocation().Z;
	}

	
}

// Called every frame
void AChargingStation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsRaycastingEnabled && !bIsLoading && !bLoadedFuelTank)
	{
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		if (PlayerController)
		{
			FVector2D MousePosition;
			if (PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y))
			{
				FVector WorldLocation, WorldDirection;
				if (PlayerController->DeprojectScreenPositionToWorld(MousePosition.X, MousePosition.Y, WorldLocation, WorldDirection))
				{
					FVector StartPoint = WorldLocation;
					FVector EndPoint = StartPoint + (WorldDirection * 10000.0f); 

					AChargingStation* ChargingStation = GetChargingStationFromRayCast(StartPoint, EndPoint);
					
					if(ChargingStation)
					{
						RayCastFindEvent.Broadcast();
						FuelTankLoaded();
						bIsLoading = true;
					}
					DisableRayCast();
				}
				
				
			}
		}
	}

}

AChargingStation* AChargingStation::GetChargingStationFromRayCast(const FVector& StartPoint, const FVector& EndPoint) const
{
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartPoint, EndPoint, ECC_Visibility, CollisionParams);

	if (bHit)
	{
		AActor* HitActor = HitResult.GetActor();
		if (HitActor)
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitActor->GetName());
		}
		
		AChargingStation* HitChargingStation = Cast<AChargingStation>(HitActor);
		if (HitChargingStation)
		{
			return HitChargingStation;
		}
		
	}
	
	return nullptr;
}

void AChargingStation::EnableRayCast()
{
	bIsRaycastingEnabled = true;
}

void AChargingStation::DisableRayCast()
{
	bIsRaycastingEnabled = false;
}

//  fuel tank Loading logic 
void AChargingStation::FuelTankLoaded()
{
	FuelTankMesh->SetVisibility(true);
	GetWorld()->GetTimerManager().SetTimer(OpacityTimerHandle, this, &AChargingStation::IncreaseOpacity, GetWorld()->GetDeltaSeconds(), true);
}

void AChargingStation::IncreaseOpacity()
{
	float TimeFraction = GetWorld()->GetDeltaSeconds(); 
	if (!DynamicTransparentMaterial) {
		UE_LOG(LogTemp, Warning, TEXT("DynamicTransparentMaterial is null"));
		return;
	}
	CurrentOpacity += TimeFraction; 
	if (CurrentOpacity >= 1.f)
	{
		CurrentOpacity = 1.f;
		GetWorld()->GetTimerManager().ClearTimer(OpacityTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(PositionTimerHandle, this, &AChargingStation::LowerFuelTank,  GetWorld()->GetDeltaSeconds(), true);
	}
	DynamicTransparentMaterial->SetScalarParameterValue(FName("Opacity"), CurrentOpacity);
}




void AChargingStation::LowerFuelTank()
{
	if (!GetWorld()) return;

	FVector Location = FuelTankMesh->GetComponentLocation();
	float TimeFraction = GetWorld()->GetDeltaSeconds(); 

	PositionTimeElapsed += TimeFraction; 

	if (PositionTimeElapsed <= 1.0f) {
		float TotalMove = 70.0f * (PositionTimeElapsed / 1.0f); 
		Location.Z = InitialZ - TotalMove; 
		FuelTankMesh->SetWorldLocation(Location);
	} else {
			GetWorld()->GetTimerManager().ClearTimer(PositionTimerHandle);
			Location.Z = InitialZ - 70.f;
			FuelTankMesh->SetWorldLocation(Location);
			bLoadedFuelTank = true;
			bIsInteractable = true;
			bIsLoading = false;
			
			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			if (PlayerController)
			{
				AFIT3039Character* PlayerCharacter = Cast<AFIT3039Character>(PlayerController->GetPawn());
				if (PlayerCharacter && PlayerCharacter->GetIsOverLappingWithChargingStation())
				{
					InteractUIComponent->SetVisibility(true);
				}
			}
		}
	}





void AChargingStation::FuelTankUsed()
{
	bIsInteractable = false;

	InteractUIComponent->SetVisibility(false);
	PositionTimeElapsed = 0.f;
	GetWorld()->GetTimerManager().SetTimer(FuelTankDisappearTimerHandle, this, &AChargingStation::DecreaseOpacity, GetWorld()->GetDeltaSeconds(), true);
}

void AChargingStation::DecreaseOpacity()
{
	float TimeFraction = GetWorld()->GetDeltaSeconds(); 
	if (!DynamicTransparentMaterial) {
		UE_LOG(LogTemp, Warning, TEXT("DynamicTransparentMaterial is null"));
		return;
	}
	CurrentOpacity -= TimeFraction; 
	if (CurrentOpacity <= 0.f)
	{
		CurrentOpacity = 0.f;
		GetWorld()->GetTimerManager().ClearTimer(FuelTankDisappearTimerHandle);
		bLoadedFuelTank = false;
		
		FVector Location = FuelTankMesh->GetComponentLocation();
		Location.Z = InitialZ;
		FuelTankMesh->SetWorldLocation(Location); //reset to original
	}
	DynamicTransparentMaterial->SetScalarParameterValue(FName("Opacity"), CurrentOpacity);
	
}
	


void AChargingStation::CheckQTE()
{
	if (bIsCharging && !bQTEChecked)
	{
		UE_LOG(LogTemp, Warning, TEXT("QTE missed"));
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_CheckQTE);
		QTEMissedEvent.Broadcast();
	}
	bQTEChecked = false;
	// Reset the flag for the next check
}

void AChargingStation::SetQTEChecked(bool bChecked)
{
	bQTEChecked = bChecked;
}