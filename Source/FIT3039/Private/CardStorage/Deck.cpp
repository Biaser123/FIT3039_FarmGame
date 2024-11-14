// Fill out your copyright notice in the Description page of Project Settings.


#include "FIT3039/Public/CardStorage/Deck.h"

#include "Cards/Card.h"

// Sets default values for this component's properties
UDeck::UDeck()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	static ConstructorHelpers::FObjectFinder<UDataTable> CardDataObject(TEXT("/Game/Cards/CardDataTable"));
	if (CardDataObject.Succeeded())
	{
		CardDataTable = CardDataObject.Object;
	}
	
	if (CardDataTable == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("cant find the datatable"));
		return;
	}

	
	/*
	TArray<FCardData*> AllRows;
	static const FString ContextString(TEXT("获取所有行测试"));

	CardDataTable->GetAllRows<FCardData>(ContextString, AllRows);
	UE_LOG(LogTemp, Warning, TEXT("数据表中的行数: %d"), AllRows.Num());

	for (const FCardData* Row : AllRows)
	{
		if (Row != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("行名: %s, 卡牌名: %s, 类型: %d, 描述: %s, 攻击力: %d, 生命值: %d, 兵种: %d"), 
				*Row->CardName, *Row->CardDescription, static_cast<int32>(Row->CardType), *Row->CardDescription, Row->AttackPower, Row->Health, static_cast<int32>(Row->TroopType));
		}
	}
	*/
	
}


// Called when the game starts
void UDeck::BeginPlay()
{
	Super::BeginPlay();
	InitializeCommodityCards();
}


// Called every frame
void UDeck::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UDeck::AddCardToDeck(const FName RowName)
{
	if (CardDataTable == nullptr) return; // Ensure the data table exists

	static const FString ContextString(TEXT("Card Lookup Context"));
	const FCardData* CardData = CardDataTable->FindRow<FCardData>(RowName, ContextString, true);

	if (CardData != nullptr)
	{
		// Add the found card data to the deck
		CardsInDeck.Add(*CardData);
	}
}

void UDeck::InitializeCommodityCards()
{
    if (CardDataTable == nullptr) return;

    TArray<FName> RowNames = CardDataTable->GetRowNames();
    FMath::RandInit(FDateTime::Now().GetTicks()); // random

    // Filter Pet, Seed, and Potion cards
    TArray<FName> SeedRowNames;
    TArray<FName> PetRowNames;
    TArray<FName> PotionRowNames;

    for (const FName& RowName : RowNames)
    {
        const FCardData* CardData = CardDataTable->FindRow<FCardData>(RowName, TEXT("InitializeCommodityCards"), true);
        if (CardData)
        {
            if (CardData->CardType == ECardType::Seed)
            {
                SeedRowNames.Add(RowName);
            }
            else if (CardData->CardType == ECardType::Pet)
            {
                PetRowNames.Add(RowName);
            }
            else if (CardData->CardType == ECardType::Potion)
            {
                PotionRowNames.Add(RowName);
            }
        }
    }

    if (SeedRowNames.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No Seed cards found in the data table"));
        return;
    }

    // Ensure at least one Seed card
    int32 SeedIndex = FMath::RandRange(0, SeedRowNames.Num() - 1);
    FName SeedRowName = SeedRowNames[SeedIndex];
    const FCardData* SeedCardData = CardDataTable->FindRow<FCardData>(SeedRowName, TEXT("InitializeCommodityCards"), true);
    if (SeedCardData)
    {
        CommodityCard1 = *SeedCardData;
    }

    // Remove the selected Seed card from the pool
    SeedRowNames.Remove(SeedRowName);

    // Randomly select the remaining two cards with probabilities
    for (int32 i = 1; i < 3; i++)
    {
        float RandomValue = FMath::FRand();
        TArray<FName>* SelectedArray = nullptr;

        if (RandomValue < 0.6f && SeedRowNames.Num() > 0)
        {
            SelectedArray = &SeedRowNames;
        }
        else if (RandomValue < 0.8f && PetRowNames.Num() > 0)
        {
            SelectedArray = &PetRowNames;
        }
        else if (PotionRowNames.Num() > 0)
        {
            SelectedArray = &PotionRowNames;
        }
        else if (PetRowNames.Num() > 0) // fallback if Potion is empty
        {
            SelectedArray = &PetRowNames;
        }
        else if (SeedRowNames.Num() > 0) // fallback if both Potion and Pet are empty
        {
            SelectedArray = &SeedRowNames;
        }

        if (SelectedArray && SelectedArray->Num() > 0)
        {
            int32 RandomIndex = FMath::RandRange(0, SelectedArray->Num() - 1);
            FName RowName = (*SelectedArray)[RandomIndex];
            const FCardData* CardData = CardDataTable->FindRow<FCardData>(RowName, TEXT("InitializeCommodityCards"), true);
            if (CardData)
            {
                switch (i)
                {
                case 1:
                    CommodityCard2 = *CardData;
                    break;
                case 2:
                    CommodityCard3 = *CardData;
                    break;
                }
            }
            SelectedArray->RemoveAt(RandomIndex);
        }
    }
    RefreshShopDelegate.Broadcast();
}


bool UDeck::PurchaseCard(int32 CardIndex, AFIT3039Character* Player, FName& OutRowName)
{
	if (!Player) return false;
	FCardData* CardToPurchase = nullptr;
    
	switch (CardIndex)
	{
	case 1: CardToPurchase = &CommodityCard1; OutRowName = CommodityCard1.RowName; break;
	case 2: CardToPurchase = &CommodityCard2; OutRowName = CommodityCard2.RowName; break;
	case 3: CardToPurchase = &CommodityCard3; OutRowName = CommodityCard3.RowName; break;
	default: return false;
	}

	if (CardToPurchase->CardName.IsEmpty())
	{
		return false; // Card has been purchased already, so return false
	}
	
	if (CardToPurchase && Player->GetGold() >= CardToPurchase->CardPrice)
	{
		Player->DecreaseGold(CardToPurchase->CardPrice); // Decrease the gold
		CardsInDeck.Remove(*CardToPurchase); // Remove card from deck
		// OutRowName is already set above in the switch statement
		*CardToPurchase = FCardData(); // Clear the bought card
		CheckAndRefreshCommodities(); // Check if needs refresh
        
		return true;
	}
    
	return false;
}




void UDeck::RefreshCommodities()
{
	
	InitializeCommodityCards(); // refresh
}

void UDeck::CheckAndRefreshCommodities()
{
	if (CommodityCard1.CardName.IsEmpty() && CommodityCard2.CardName.IsEmpty() && CommodityCard3.CardName.IsEmpty())
	{
		RefreshCommodities(); // if no card then refresh
	}
}