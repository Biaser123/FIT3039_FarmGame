// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridNode.h"
#include "Components/StaticMeshComponent.h"

#include "Crops.h"
#include "PetCharacter.h"
#include "CardStorage/Shop.h"
#include "GridMap.generated.h"

class UProceduralMeshComponent;
class UMaterial;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterSpawnedDelegate);

UCLASS(HideCategories=(Materials))
class FIT3039_API AGridMap : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGridMap();

	//spawn crops
	UPROPERTY(BlueprintAssignable)
	FOnCharacterSpawnedDelegate OnCropSpawned;

	//spawn pets
	UPROPERTY(BlueprintAssignable)
	FOnCharacterSpawnedDelegate OnPetSpawned;

	UPROPERTY(BlueprintAssignable)
	FOnCharacterSpawnedDelegate OnFertilizingEvent;
	
	//基础-棋盘单位大小
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Size;

	//基础-棋盘行数
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Roll;

	//setting in editor
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Column;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	float DistanceX; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	float DistanceY;


	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Properties")
	// UStaticMesh* NodeMesh;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Properties")
	// UMaterialInterface* NodeMaterial;

	//基础-棋盘的棋格
	UPROPERTY()
	TMap<FGridVector, UGridNode*> NodeMap;

	//基础-生成棋盘
	UFUNCTION(BlueprintCallable)
	void GenerateGridMap();

	UFUNCTION(BlueprintCallable)
	UGridNode* GetNode(FGridVector InCoord) const;

	//基础-生成棋格入口
	UFUNCTION()
	void GenerateNodes(float InSize, int InRoll, int InColumn);

	//基础-初始化棋格入口
	UFUNCTION()
	void InitNodes();
	
	//模型-棋盘模型
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UProceduralMeshComponent* MapMesh;

	//模型-可以放置格子的材质
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* NormalMaterial;

	//模型-不可以放置格子的材质
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* HighlightMaterial;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* DebugMaterial1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* DebugMaterial2;

    //default time for pet spawning
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float PetSpawnTime = 5.0f;

	//raycast for spawning crop
	UFUNCTION(BlueprintCallable, Category = "GridMap")
	void EnableRaycast();

	UFUNCTION(BlueprintCallable, Category = "GridMap")
	void DisableRaycast();

	//raycast for fertilizing crop
	UFUNCTION(BlueprintCallable, Category = "GridMap")
	void EnableFertilizingRayCast();

	UFUNCTION(BlueprintCallable, Category = "GridMap")
	void DisableFertilizingRayCast();


	//raycast for pet
	UFUNCTION(BlueprintCallable, Category = "GridMap")
	void EnablePetRayCast();

	UFUNCTION(BlueprintCallable, Category = "GridMap")
	void DisablePetRayCast();

	
	//模型-生成棋盘模型
	UFUNCTION(BlueprintCallable)
	void GenerateMapMesh();

	//棋格碰撞体积创建
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<UBoxComponent*> CollisionComponents;

	//棋格碰撞体积映射到对应棋格
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TMap<UBoxComponent*, UGridNode*> CollisionComponentToNodeMap;

	//Ray cast-get node
	UFUNCTION(BlueprintCallable)
	UGridNode *GetNodeFromRaycast(const FVector& StartPoint, const FVector& EndPoint) const;

	UFUNCTION(BlueprintCallable)
	AShop* GetShopFromRaycast(const FVector& StartPoint, const FVector& EndPoint) const;
	
	// 当前被射线照射的棋格
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UGridNode* CurrentHoveredNode = nullptr;
 
	// Crop interaction
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Crop")
	TSubclassOf<ACrop> BP_CropClass;

	// Crop interaction
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Pet")
	TSubclassOf<APetCharacter> BP_PetClass;

	// pet spawn
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Crop")
	bool IsPetSpawn = false;

	// crop spawn
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Crop")
	bool IsCropSpawn = false;
	
	//tracer-deal hovered Node
	UFUNCTION(BlueprintCallable, Category = "GridMap")
	void OnGridNodeHovered(UGridNode* HoveredNode);

	//tracer-deal hovered shop
	UFUNCTION(BlueprintCallable, Category = "GridMap")
	void OnShopHovered(AShop* HoveredShop);

	UFUNCTION(BlueprintCallable, Category = "GridMap")
	void OnCropHovered(UGridNode* HoveredNode);
	
	//Material
	UFUNCTION(BlueprintCallable)
	void SetNodeMaterial(UGridNode* InNode, UMaterialInterface* InMaterial);

	//Material
	UFUNCTION(BlueprintCallable)
	void SetAllInteractableMaterial();

	//Material
	UFUNCTION(BlueprintCallable)
	void ResetNodeMaterial(UGridNode* InNode);

	//Material
	UFUNCTION(BlueprintCallable)
	void ResetNodeMaterialAll();

	UFUNCTION(BlueprintCallable)
	bool GetBisSpawning();
	
	bool IsCoordValid(const FGridVector& Coord) const;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Pet")
	float CurrentProgress = 0.f; 

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Pet")
	float PetProgressRatio = 0.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitProperties() override;

// #if WITH_EDITOR
// 	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
//
// #endif

	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	bool bIsRaycastingEnabled = false;

	bool bIsCheckForPetRayCastingEnabled = false;

	bool bIsFertilizingRayCastingEnabled = false;

	FTimerHandle TimerHandle_PetSpawn;

	bool bIsSpawning = false;

	AShop*CurrentShop;
	
protected:
	void UpdatePetSpawnProgress();
};
