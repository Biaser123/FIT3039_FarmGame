// Fill out your copyright notice in the Description page of Project Settings.


#include "CardStorage/Shop.h"

#include "FIT3039/FIT3039Character.h"

// Sets default values
AShop::AShop()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	
	Deck = CreateDefaultSubobject<UDeck>(TEXT("Deck"));

	ShopMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ShopMesh"));
	ShopMesh->SetupAttachment(RootComponent);

	DetectionSphere = CreateDefaultSubobject<UBoxComponent>(TEXT("DetectionSphere"));
	DetectionSphere->SetupAttachment(RootComponent);
	DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AShop::OnOverlapBegin);
	DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &AShop::OnOverlapEnd);

	//progression progress UI for pet spawning
	ProgressionWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("ProgressionWidget"));
	ProgressionWidgetComponent->SetupAttachment(RootComponent);
	ProgressionWidgetComponent->SetVisibility(false);
	
	InteractUIComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionIconWidget"));
	InteractUIComponent->SetupAttachment(RootComponent);
	InteractUIComponent->SetVisibility(false);

	Anim = Cast<UAnimationAsset>(StaticLoadObject(UAnimationAsset::StaticClass(), nullptr, TEXT("/Game/Animation/AS_PetSpawning.AS_PetSpawning")));
	

}

// Called when the game starts or when spawned
void AShop::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AShop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AShop::PlayPetSpawnAnimation()
{
	if (Anim != nullptr)
	{
		ShopMesh->PlayAnimation(Anim, false);
	}
	else if (Anim == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load animation asset."));
	}
}

void AShop::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AFIT3039Character* PlayerCharacter = Cast<AFIT3039Character>(OtherActor);
	if (PlayerCharacter)
	{
		PlayerCharacter->SetIsOverlappingWithShop(true);
		InteractUIComponent->SetVisibility(true); 
	}
}

void AShop::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AFIT3039Character* PlayerCharacter = Cast<AFIT3039Character>(OtherActor);
	if (PlayerCharacter)
	{
		PlayerCharacter->SetIsOverlappingWithShop(false);
		InteractUIComponent->SetVisibility(false); 
	}
}