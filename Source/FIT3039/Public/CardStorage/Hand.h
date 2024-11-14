// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FIT3039/Public/Cards/CardDataStruct.h"
#include "FIT3039/Public/Cards/GameEnum.h"
#include "Hand.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FIT3039_API UHand : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHand();


	UFUNCTION(BlueprintCallable, Category = "Cards")
	EDrawCardResult DrawCards(const FName RowName, int Card_Num, FCardData& OutCardData);

	UFUNCTION(BlueprintCallable)
	EUseCardResult UseCard(const FCardData& UsedCard);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cards")
	TArray<FCardData> CardsInHand;
	
	
	UPROPERTY(EditDefaultsOnly, Category = "Cards")
	UDataTable* CardDataTable;
	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
		
};
