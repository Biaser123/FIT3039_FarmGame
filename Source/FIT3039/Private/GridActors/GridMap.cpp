// Fill out your copyright notice in the Description page of Project Settings.


#include "GridActors/GridMap.h"
#include "ProceduralMeshComponent.h"
#include "GridActors/PetCharacter.h"


// Sets default values
AGridMap::AGridMap()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	if (!MapMesh)
	{
		MapMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("MapMesh"));
		if (MapMesh)
		{
			MapMesh->SetupAttachment(RootComponent);
		}
	}
	PetProgressRatio = 0.f;
	
}

// Called when the game starts or when spawned
void AGridMap::BeginPlay()
{
	Super::BeginPlay();
	//generate grid map
	GenerateGridMap();
	
}

//initial
void AGridMap::PostInitProperties()
{
	Super::PostInitProperties();
	GenerateGridMap();
}


// Called every frame
void AGridMap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// check for the crop spawning
	if (bIsRaycastingEnabled)
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

					UGridNode* HoveredNode = GetNodeFromRaycast(StartPoint, EndPoint);
					
					if(HoveredNode)
					{
						OnGridNodeHovered(HoveredNode);
					}
					DisableRaycast();
				}
				
				
			}
		}
	}
	else if(bIsCheckForPetRayCastingEnabled)
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

					AShop* Shop = GetShopFromRaycast(StartPoint, EndPoint);
					
					if (Shop && BP_PetClass!= nullptr && !bIsSpawning)
					{
						OnPetSpawned.Broadcast();
						bIsSpawning = true;
						CurrentShop = Shop;

						Shop->ProgressionWidgetComponent->SetVisibility(true);
						GetWorld()->GetTimerManager().SetTimer(TimerHandle_PetSpawn, this, &AGridMap::UpdatePetSpawnProgress, 1.0f / 60.0f, true);
					}
					DisablePetRayCast();
				}
			}
		}
	}
	else if (bIsFertilizingRayCastingEnabled)
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

					UGridNode* HoveredNode = GetNodeFromRaycast(StartPoint, EndPoint);
					
					if(HoveredNode)
					{
						OnCropHovered(HoveredNode);
					}
					DisableFertilizingRayCast();
				}
				
				
			}
		}
	}
}

void AGridMap::UpdatePetSpawnProgress()
{
	if (bIsSpawning)
	{
		CurrentProgress += 1.0f / 60.0f; 

		float ProgressRatio = CurrentProgress / PetSpawnTime; 
		ProgressRatio = FMath::Clamp(ProgressRatio, 0.0f, 1.0f);

		this->PetProgressRatio = ProgressRatio;

		if (CurrentProgress >= PetSpawnTime)
		{
			OnShopHovered(CurrentShop); 
			CurrentProgress = 0;
			PetProgressRatio = 0;
			CurrentShop->ProgressionWidgetComponent->SetVisibility(false);
		}
	}
}


void AGridMap::GenerateGridMap()
{
	// clean the collision
	for (UBoxComponent* OldCollisionComponent : CollisionComponents)
	{
		if (OldCollisionComponent)
		{
			OldCollisionComponent->DestroyComponent();
		}
	}
	CollisionComponents.Reset();

	// clean the map
	CollisionComponentToNodeMap.Reset();
    
	//reset and regenerate
	NodeMap.Reset();
	GenerateNodes(Size, Roll, Column);
	GenerateMapMesh();
}

void AGridMap::GenerateNodes(float InSize, int InRoll, int InColumn)
{
    int TotalColumns = InColumn;

    for (int X = 0; X < InRoll; X++)
    {
        for (int Y = 0; Y < TotalColumns; Y++)
        {
            FVector Location = FVector(X * (InSize + DistanceX), Y * (InSize + DistanceY), 0) + GetActorLocation();
            FGridVector Coordinate(X, Y);
            // set every node to empty
            EOccupantType OccupantType = EOccupantType::Ot_Empty;

            // create node
            UGridNode* Node = NewObject<UGridNode>(this, UGridNode::StaticClass());
            if (Node)
            {
                Node->InitNode(this, Location, Coordinate, InSize, OccupantType);
                NodeMap.Add(Coordinate, Node);
                
                // create collition for each node 
                UBoxComponent* CollisionComponent = NewObject<UBoxComponent>(this, UBoxComponent::StaticClass(), NAME_None, RF_NoFlags, nullptr, false);
                if (CollisionComponent)
                {
                    CollisionComponent->RegisterComponent();

                    
                    if (MapMesh && MapMesh->IsRegistered())
                    {
                        CollisionComponent->AttachToComponent(MapMesh, FAttachmentTransformRules::KeepWorldTransform);
                    }
                    else
                    {
                        UE_LOG(LogTemp, Warning, TEXT("MapMesh is not initialized or registered"));
                    }

                    CollisionComponent->SetWorldLocation(Location);
                    CollisionComponent->SetBoxExtent(FVector(InSize / 2, InSize / 2, 5)); 
                    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
                    CollisionComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

                    CollisionComponents.Add(CollisionComponent);

                    // update the map
                    CollisionComponentToNodeMap.Add(CollisionComponent, Node);
                }
            }
        }
    }
}

void AGridMap::InitNodes()
{

}



void AGridMap::EnableRaycast()
{
	bIsRaycastingEnabled = true;
}

void AGridMap::DisableRaycast()
{
	bIsRaycastingEnabled = false;
}

void AGridMap::EnableFertilizingRayCast()
{
	bIsFertilizingRayCastingEnabled = true;
}

void AGridMap::DisableFertilizingRayCast()
{
	bIsFertilizingRayCastingEnabled = false;
}

void AGridMap::OnCropHovered(UGridNode* HoveredNode)
{
	// only its empty
	if (HoveredNode->GetOccupant() == EOccupantType::Ot_Occupied)
	{
		// if not at the waiting for seeding phase
		if(!HoveredNode->OccupyCrop->Tags.Contains(TEXT("Seeding")))
		{
			HoveredNode->OccupyCrop->ClearDiseases();
			OnFertilizingEvent.Broadcast();
		}
	}
}

void AGridMap::EnablePetRayCast()
{
	bIsCheckForPetRayCastingEnabled = true;
}

void AGridMap::DisablePetRayCast()
{
	bIsCheckForPetRayCastingEnabled = false;
}


UGridNode* AGridMap::GetNode(FGridVector InCoord) const
{
	if (NodeMap.Contains(InCoord))
		return NodeMap[InCoord];
	else
		return nullptr;
}

UGridNode* AGridMap::GetNodeFromRaycast(const FVector& StartPoint, const FVector& EndPoint) const
{
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	//CollisionParams.AddIgnoredActor(this);

	// ray cast
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartPoint, EndPoint, ECC_Visibility, CollisionParams);
	//debug tracing
	/*
	DrawDebugLine(
	GetWorld(),
	StartPoint,
	EndPoint,
	FColor(255, 0, 0),
	false, // persistent lines
	0.1, // lifetime
	0, // depth priority
	1.0 // thickness
);
*/

	if (bHit && HitResult.GetComponent())
	{
		// 如果命中了某个组件，尝试找到对应的GridNode
		UBoxComponent* HitBoxComponent = Cast<UBoxComponent>(HitResult.GetComponent());
		if (HitBoxComponent)
		{
			// 直接使用UGridNode*来接收Find的结果
			UGridNode* const* FoundNode = CollisionComponentToNodeMap.Find(HitBoxComponent);
			if (FoundNode)
			{
				// 如果找到了对应的GridNode，返回它
				return *FoundNode; // 使用*FoundNode来获取UGridNode*指向的对象
			}
		}
	}
	// 如果没有命中或找不到对应的GridNode，返回nullptr
	return nullptr;
}

AShop* AGridMap::GetShopFromRaycast(const FVector& StartPoint, const FVector& EndPoint) const
{
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartPoint, EndPoint, ECC_Visibility, CollisionParams);

	if (bHit)
	{
		AActor* HitActor = HitResult.GetActor();
	
		
		AShop* HitShop = Cast<AShop>(HitActor);
		if (HitShop)
		{
			return HitShop;
		}
		
	}
	
	return nullptr;
}




void AGridMap::OnGridNodeHovered(UGridNode* HoveredNode)
{
	// only its empty
	if (HoveredNode->GetOccupant() == EOccupantType::Ot_Empty)
	{
		if (BP_CropClass != nullptr)
		{
			FVector Location = HoveredNode->Location;
			FRotator Rotation(0.0f, 90.0f, 0.0f); 
			FVector SpawnLocation = Location;
			SpawnLocation.Z += 6;
			ACrop* Crop = GetWorld()->SpawnActor<ACrop>(BP_CropClass, SpawnLocation, Rotation);
			if(Crop)
			{
				Crop->SetOccupyingNode(HoveredNode);
				Crop->GetNeighborsNodes();
			}
			HoveredNode->SetOccupyingPlant(Crop);
			
			HoveredNode ->SetOccupant(EOccupantType::Ot_Occupied);
			IsCropSpawn = true;
			OnCropSpawned.Broadcast();
			ResetNodeMaterialAll();
		}
		
	}
}

void AGridMap::OnShopHovered(AShop* HoveredShop)
{
	if(HoveredShop && BP_PetClass!= nullptr && bIsSpawning)
	{
		HoveredShop->PlayPetSpawnAnimation();
		
		FVector Location = HoveredShop->GetActorLocation();
		FRotator Rotation(0.0f, 90.0f, 0.0f);
		FVector SpawnLocation = Location;
		SpawnLocation.Y += 340;
		SpawnLocation.Z += 100;
		IsPetSpawn = true;
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, SpawnLocation, Rotation]() {
			APetCharacter* Pet = GetWorld()->SpawnActor<APetCharacter>(BP_PetClass, SpawnLocation, Rotation);bIsSpawning = false;
		}, 1.1f, false);
	}
}

void AGridMap::GenerateMapMesh()
{
	int Index = 0;
	MapMesh->ClearAllMeshSections();
	for (auto a : NodeMap)
	{
		if (!a.Value)
			continue;
		TArray<FVector> Vertices;
		TArray<int32> Indecies;
		TArray<FVector> Normals;
		TArray<FVector2D> UV1, EmptyArray;
		TArray<FColor> VertexColors;
		TArray<FVector> Tangents;
		TArray<FProcMeshTangent> MeshTangents;
		a.Value->MeshIndex = Index;
		a.Value->DrawNode(Vertices, Indecies, Normals, UV1, VertexColors, Tangents, GetActorLocation());
		for (FVector& b : Tangents)
			MeshTangents.Add(FProcMeshTangent(b, false));
		MapMesh->CreateMeshSection(Index, Vertices, Indecies, Normals, UV1, EmptyArray, EmptyArray, EmptyArray, VertexColors, MeshTangents, false);
		//reset
		ResetNodeMaterial(a.Value);
		Index++;
	}
}

//set material
void AGridMap::SetNodeMaterial(UGridNode* InNode, UMaterialInterface* InMaterial)
{
	if (!InNode || !InMaterial)
		return;
	MapMesh->SetMaterial(InNode->MeshIndex, InMaterial);
}



//reset Material
void AGridMap::ResetNodeMaterial(UGridNode* InNode)
{

	if(!InNode)
		return;
	SetNodeMaterial(InNode,NormalMaterial);
	
}

void AGridMap::SetAllInteractableMaterial()
{
	for (auto a : NodeMap)
		{
		if (!a.Value)
			continue;
		if(a.Value->GetOccupant() == EOccupantType::Ot_Empty)
		SetNodeMaterial(a.Value,HighlightMaterial);
		}
}

//reset all materials
void AGridMap::ResetNodeMaterialAll()
{
	for (auto a : NodeMap)
	{
		if (!a.Value)
			continue;
		ResetNodeMaterial(a.Value);
	}
}

bool AGridMap::GetBisSpawning()
{
	return bIsSpawning;
}

//check if the coordinate is valid 
bool AGridMap::IsCoordValid(const FGridVector& Coord) const {
	return Coord.X >= 0 && Coord.X < Roll && Coord.Y >= 0 && Coord.Y < Column;
}



