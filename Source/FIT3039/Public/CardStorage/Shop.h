// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Actor.h"

#include "FIT3039/Public/CardStorage/Deck.h"
#include "Shop.generated.h"

UCLASS()
class FIT3039_API AShop : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShop();

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Mesh")
	USkeletalMeshComponent* ShopMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI Component")
	UWidgetComponent* InteractUIComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI Component")
	UWidgetComponent* ProgressionWidgetComponent;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Trigger")
	UBoxComponent* DetectionSphere;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite,Category = Logic_Component)
	class UDeck* Deck;

	UFUNCTION()
	void PlayPetSpawnAnimation();
	// Function called when a player begins overlapping with the detection sphere
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Function called when a player ends overlapping with the detection sphere
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UAnimationAsset* Anim;
};
