// Fill out your copyright notice in the Description page of Project Settings.


#include "BalancingValue.h"

#include "GridActors/Crops.h"
#include "GridActors/PetCharacter.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ABalancingValue::ABalancingValue()
{
	
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DailyOxygenDecrease = 0.0f;
	NightTemperatureDrop = 0.0f;
	// Initialize default values
	BaseTemperature = 10.0f;
	CurrentTemperature = BaseTemperature;
	BaseOxygenLevel = 21.0f;
	CurrentOxygenLevel = BaseOxygenLevel;
}

	

// Called when the game starts or when spawned
void ABalancingValue::BeginPlay()
{
	Super::BeginPlay();

	//bind all these delegete
	TimeManager = Cast<ATimeManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ATimeManager::StaticClass()));
	if (TimeManager)
	{
		TimeManager->OnNightStarted.AddDynamic(this, &ABalancingValue::HandleNightStarted);
		TimeManager->OnDayStarted.AddDynamic(this, &ABalancingValue::HandleDayStarted);
		// Connect to the day changed event
		TimeManager->OnDayChanged.AddDynamic(this, &ABalancingValue::HandleDayChanged);
	}
	
}

// Called every frame
void ABalancingValue::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateEnvironment();

	if(GetTemperatureStatus() == ETemperatureStatus::Deadly || GetOxygenLevelStatus() == EOxygenLevelStatus::Deadly)
	{
		FGameOver.Broadcast();
	}
	else if(GetTemperatureStatus() == ETemperatureStatus::Affected || GetOxygenLevelStatus() == EOxygenLevelStatus::Affected)
	{
		FDangerWarning.Broadcast();
	}

}


void ABalancingValue::UpdateEnvironmentValues()
{
	// Here you would add logic to update the environment values dynamically
	// For now, this function can be used to recalculate the current values based on external factors
}

ETemperatureStatus ABalancingValue::GetTemperatureStatus() const
{
	if (CurrentTemperature >= 10.0f && CurrentTemperature <= 30.0f)
	{
		return ETemperatureStatus::Normal;
	}
	else if ((CurrentTemperature >= -20.0f && CurrentTemperature < 10.0f) || (CurrentTemperature > 30.0f && CurrentTemperature <= 50.0f))
	{
		return ETemperatureStatus::Affected;
	}
	else
	{
		return ETemperatureStatus::Deadly;
	}
}

EOxygenLevelStatus ABalancingValue::GetOxygenLevelStatus() const
{
	if (CurrentOxygenLevel >= 19.5f && CurrentOxygenLevel <= 23.5f)
	{
		return EOxygenLevelStatus::Normal;
	}
	else if ((CurrentOxygenLevel >= 10.0f && CurrentOxygenLevel < 19.5f) || (CurrentOxygenLevel > 23.5f && CurrentOxygenLevel <= 40.f))
	{
		return EOxygenLevelStatus::Affected;
	}
	else
	{
		return EOxygenLevelStatus::Deadly;
	}
}

void ABalancingValue::UpdateEnvironment()
{
	// reset to normal
	CurrentTemperature = BaseTemperature;
	CurrentOxygenLevel = BaseOxygenLevel;
	CropTemperature = 0;
	CropOxygenLevel = 0;
	PetOxygenLevel = 0;

	// get all crops
	for (TActorIterator<ACrop> It(GetWorld()); It; ++It)
	{
		ACrop* Crop = *It;
		if (Crop && !Crop->Tags.Contains("Seeding"))
		{
			// + all
			CurrentTemperature += Crop->TemperatureValue;
			CropTemperature += Crop->TemperatureValue;
			
			CurrentOxygenLevel += Crop->OxygenValue;
			CropOxygenLevel += Crop->OxygenValue;
		}
	}

	// get all crops
	for (TActorIterator<APetCharacter> It(GetWorld()); It; ++It)
	{
		APetCharacter* Pet = *It;
		if (Pet)
		{
			CurrentOxygenLevel += Pet->OxygenValue;
			PetOxygenLevel += Pet->OxygenValue;
		}
	}

	// limit min, max
	CurrentTemperature = FMath::Clamp(CurrentTemperature, -50.0f, 50.0f);
	CurrentOxygenLevel = FMath::Clamp(CurrentOxygenLevel, 0.0f, 100.0f);
}

void ABalancingValue::HandleNightStarted()
{
	BaseTemperature -= NightTemperatureDrop;
}

void ABalancingValue::HandleDayStarted()
{
	BaseTemperature += NightTemperatureDrop;
}

void ABalancingValue::HandleDayChanged()
{
	BaseOxygenLevel -= DailyOxygenDecrease;
	BaseOxygenLevel = FMath::Max(BaseOxygenLevel, 0.0f);  // Ensure it doesn't go below 0
}