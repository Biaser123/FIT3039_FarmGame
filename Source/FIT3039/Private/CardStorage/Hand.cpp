// Fill out your copyright notice in the Description page of Project Settings.


#include "FIT3039/Public/CardStorage/Hand.h"

// Sets default values for this component's properties
UHand::UHand()
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
		UE_LOG(LogTemp, Warning, TEXT("Cant find data table"));
		return;
	}
}


EDrawCardResult UHand::DrawCards(const FName RowName, const int Card_Num, FCardData& OutCardData)
{
	if (CardDataTable == nullptr)
		return EDrawCardResult::NotFound; // return if cant found card table

	static const FString ContextString(TEXT("Card Lookup Context"));
	const FCardData* CardData = CardDataTable->FindRow<FCardData>(RowName, ContextString, true);

	if (CardData != nullptr)
	{
		for (FCardData& ExistingCard : CardsInHand)
		{
			if (ExistingCard.CardName == CardData->CardName)
			{
				// add num when found card
				ExistingCard.CardNum += Card_Num;
				OutCardData = ExistingCard; // update the new card num 
				return EDrawCardResult::IncreasedQuantity;
			}
		}

		// if not found the card, add a new card to the hand
		FCardData NewCard = *CardData;
		NewCard.CardNum = Card_Num; // set the initial card num
		CardsInHand.Add(NewCard);
		OutCardData = NewCard; // update the card data
		return EDrawCardResult::NewCard;
	}
	else
	{
		return EDrawCardResult::NotFound;
	}
}


EUseCardResult UHand::UseCard(const FCardData& UsedCard)
{
	int32 IndexToRemove = -1;
	for (int32 i = 0; i < CardsInHand.Num(); ++i)
	{
		if (CardsInHand[i].CardName == UsedCard.CardName)
		{
			if (CardsInHand[i].CardNum > 1)
			{
				CardsInHand[i].CardNum -= 1;
				return EUseCardResult::DecreaseQuantity;
			}
			else
			{
				IndexToRemove = i;
				break;
			}
		}
	}

	if (IndexToRemove != -1)
	{
		CardsInHand.RemoveAt(IndexToRemove);
		return EUseCardResult::OnlyOneLast;
	}

	return EUseCardResult::NoCards;
}




// Called when the game starts
void UHand::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UHand::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

