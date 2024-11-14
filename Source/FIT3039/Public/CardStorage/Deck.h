// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Components/ActorComponent.h"
#include "FIT3039/FIT3039Character.h"
#include "FIT3039/Public/Cards/CardDataStruct.h"
#include "Deck.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRefreshShopDelegate);
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FIT3039_API UDeck : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDeck();

	// CardList
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Deck")
	TArray<FCardData> CardsInDeck;
	
	// DataTable
	UPROPERTY(EditDefaultsOnly, Category = "Cards")
	UDataTable* CardDataTable;
	
	void AddCardToDeck(const FName RowName);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Deck")
	FCardData CommodityCard1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Deck")
	FCardData CommodityCard2;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Deck")
	FCardData CommodityCard3;

	UFUNCTION(BlueprintCallable)
	void InitializeCommodityCards();
	
	void RefreshCommodities();
	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FRefreshShopDelegate  RefreshShopDelegate;
	
	void CheckAndRefreshCommodities();
	
	UFUNCTION(BlueprintCallable)
	bool PurchaseCard(int32 CardIndex, AFIT3039Character* Player, FName& OutRowName);

		
};


