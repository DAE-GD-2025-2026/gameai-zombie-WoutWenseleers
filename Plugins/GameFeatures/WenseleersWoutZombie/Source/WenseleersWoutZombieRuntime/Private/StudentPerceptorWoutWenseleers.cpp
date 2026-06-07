// Fill out your copyright notice in the Description page of Project Settings.


#include "StudentPerceptorWoutWenseleers.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Common/InventoryComponent.h"

#include "Zombies/BaseZombie.h"
#include "Items/BaseItem.h"
#include "Items/ItemType.h"
#include "Village/House/House.h"

UStudentPerceptorWoutWenseleers::UStudentPerceptorWoutWenseleers()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UStudentPerceptorWoutWenseleers::BeginPlay()
{
	Super::BeginPlay();
	
	if (auto PerceptionComp = GetOwner()->GetComponentByClass<UAIPerceptionComponent>())
	{
		PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &UStudentPerceptorWoutWenseleers::OnPerceptionUpdated);
	}

	if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
	{
		AIController = Cast<AAIController>(OwnerPawn->GetController());
	}

	if (!AIController)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("ERROR: AIController == nullptr"));
		return;
	}

	BlackboardComp = AIController->GetBlackboardComponent();
	if (!BlackboardComp)
	{
		GEngine->AddOnScreenDebugMessage(-2, 3.f, FColor::Red, TEXT("ERROR: Blackboard == nullptr "));
		return;
	}
}

//void UStudentPerceptor::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
//{
//	if (!Actor || !Stimulus.IsValid() || !Stimulus.WasSuccessfullySensed())
//	{
//		return;
//	}
//	if (!AIController)
//	{
//		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("ERROR: AIController == nullptr"));
//		return;
//	}
//	if (!BlackboardComp)
//	{
//		GEngine->AddOnScreenDebugMessage(-2, 3.f, FColor::Red, TEXT("ERROR: Blackboard == nullptr "));
//		return;
//	}
//
//	if (ABaseItem* SeenItem = Cast<ABaseItem>(Actor))
//	{
//		if (SeenItem->GetItemType() != EItemType::Garbage)
//	}
//	
//	GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Yellow, 
//	FString::Printf(TEXT("Seen Actor: %s"), *Actor->GetName()));
//	
//}

void UStudentPerceptorWoutWenseleers::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor || !Stimulus.IsValid())
	{
		return;
	}

	if (!AIController || !BlackboardComp)
	{
		return;
	}

	const bool bSuccessfullySensed = Stimulus.WasSuccessfullySensed();

	RefreshSurvivalState();

	if (Actor->IsA<ABaseZombie>())
	{
		HandleZombiePerception(Actor, bSuccessfullySensed);
		return;
	}

	if (ABaseItem* Item = Cast<ABaseItem>(Actor))
	{
		HandleItemPerception(Item, bSuccessfullySensed);
		return;
	}

	if (AHouse* House = Cast<AHouse>(Actor))
	{
		HandleHousePerception(House, bSuccessfullySensed);
		return;
	}
}

void UStudentPerceptorWoutWenseleers::HandleZombiePerception(AActor* Actor, bool bSuccessfullySensed)
{
	if (!BlackboardComp || !Actor)
	{
		return;
	}

	if (bSuccessfullySensed)
	{
		BlackboardComp->SetValueAsObject(TargetZombieKey, Actor);
		BlackboardComp->SetValueAsVector(LastKnownZombieLocationKey, Actor->GetActorLocation());
		BlackboardComp->SetValueAsBool(HasZombieKey, true);

		GEngine->AddOnScreenDebugMessage(
			1,
			1.5f,
			FColor::Red,
			FString::Printf(TEXT("Zombie seen: %s"), *Actor->GetName())
		);
	}
	else
	{
		ClearActorKeyIfMatching(TargetZombieKey, Actor);

		if (!BlackboardComp->GetValueAsObject(TargetZombieKey))
		{
			BlackboardComp->SetValueAsBool(HasZombieKey, false);
		}
	}
}

void UStudentPerceptorWoutWenseleers::HandleItemPerception(ABaseItem* Item, bool bSuccessfullySensed)
{
	if (!Item)
	{
		return;
	}

	if (bSuccessfullySensed)
	{
		RegisterKnownItem(Item);
	}
	else
	{
		//if (!IsValid(Item) || !IsUsefulItem(Item))
		//{
		//	ForgetKnownItem(Item);
		//}
	}

	RefreshItemTarget();
}

void UStudentPerceptorWoutWenseleers::RegisterKnownItem(ABaseItem* Item)
{
	if (!IsUsefulItem(Item))
	{
		return;
	}

	KnownItems.AddUnique(Item);
}

void UStudentPerceptorWoutWenseleers::ForgetKnownItem(ABaseItem* Item)
{
	if (!Item)
	{
		return;
	}

	KnownItems.Remove(Item);
	ClearActorKeyIfMatching(TargetItemKey, Item);
}

void UStudentPerceptorWoutWenseleers::HandleHousePerception(AHouse* House, bool bSuccessfullySensed)
{
	if (!House)
	{
		return;
	}

	if (bSuccessfullySensed)
	{
		RegisterKnownHouse(House);
	}
	else
	{
		// Do not remove the house just because it is no longer visible.
		// The AI remembers perceived houses as search targets.
	}

	RefreshHouseTarget();
}

void UStudentPerceptorWoutWenseleers::RegisterKnownHouse(AHouse* House)
{
	if (!IsValid(House))
	{
		return;
	}

	if (!KnownHouses.Contains(House))
	{
		KnownHouses.AddUnique(House);

		KnownUnvisitedHouses.Add(House);

		GEngine->AddOnScreenDebugMessage(
			3,
			2.0f,
			FColor::Cyan,
			FString::Printf(TEXT("House registered: %s"), *House->GetName())
		);
	}
}

void UStudentPerceptorWoutWenseleers::MarkHouseVisited(AHouse* House)
{
	if (!House)
	{
		return;
	}

	KnownUnvisitedHouses.Remove(House);
	ClearActorKeyIfMatching(TargetHouseKey, House);
	RefreshHouseTarget();
}


int32 UStudentPerceptorWoutWenseleers::GetItemPriority(const ABaseItem* Item) const
{
	if (!Item)
	{
		return -1;
	}

	// Ignore already-used or empty items.
	if (Item->GetValue() <= 0)
	{
		return -1;
	}

	ASurvivorPawn* const Survivor = Cast<ASurvivorPawn>(AIController->GetPawn());
	if (!Survivor) return -1;
    
	UInventoryComponent* const Inventory{Survivor->GetComponentByClass<UInventoryComponent>()};
	if (!Inventory) return -1;

	int nrWeapons{};
	int nrAmmo{};
	int nrFood{};
	int nrMedkit{};

	for (ABaseItem* item : Inventory->GetInventory())
	{
		if (!item) continue;
		switch (item->GetItemType())
		{
		case EItemType::Shotgun:
			nrAmmo += item->GetValue();
		case EItemType::Pistol:
			++nrWeapons;
			nrAmmo += item->GetValue();
			break;
		case EItemType::Medkit:
			++nrMedkit;
			break;
		case EItemType::Food:
			++nrFood;
			break;
		case EItemType::Garbage:
		default:
			break;
		}
	}

	if (nrWeapons >= 1)
	{
		BlackboardComp->SetValueAsBool(ShouldFleeCombatKey, false);
	}
	else
	{
		BlackboardComp->SetValueAsBool(ShouldFleeCombatKey, true);
	}
	
	switch (Item->GetItemType())
	{
	case EItemType::Shotgun:
		if (nrWeapons < 1 || nrAmmo < 4) return 999; // weaponless / low ammo => prio weapon
		return -1;

	case EItemType::Pistol:
		if (nrWeapons < 1 || nrAmmo < 4) return 999; // weaponless / low ammo => prio weapon
		return -1;

	case EItemType::Medkit:
		if (nrMedkit < 1) return 2;
		if (nrMedkit < 2) return 1;
		return -1;

	case EItemType::Food:
		if (nrFood < 1) return 2;
		if (nrFood < 2) return 1;
		return -1;

	case EItemType::Garbage:
	default:
		return -1;
	}
}

void UStudentPerceptorWoutWenseleers::ClearActorKeyIfMatching(FName KeyName, AActor* Actor) const
{
	if (!BlackboardComp || !Actor)
	{
		return;
	}

	if (BlackboardComp->GetValueAsObject(KeyName) == Actor)
	{
		BlackboardComp->ClearValue(KeyName);
	}
}

void UStudentPerceptorWoutWenseleers::RefreshItemTarget()
{
	if (!BlackboardComp)
	{
		return;
	}

	KnownItems.RemoveAll([](const TObjectPtr<ABaseItem>& Item)
	{
		return !IsValid(Item) || Item->GetItemType() == EItemType::Garbage;
	});

	ABaseItem* BestItem = FindBestKnownItem();

	if (BestItem)
	{
		BlackboardComp->SetValueAsObject(TargetItemKey, BestItem);
		BlackboardComp->SetValueAsVector(TargetItemLocationKey, BestItem->GetActorLocation());
		BlackboardComp->SetValueAsBool(HasItemKey, true);
	}
	else
	{
		BlackboardComp->ClearValue(TargetItemKey);
		BlackboardComp->ClearValue(TargetItemLocationKey);
		BlackboardComp->SetValueAsBool(HasItemKey, false);
	}
}

void UStudentPerceptorWoutWenseleers::RefreshHouseTarget()
{
	if (!BlackboardComp)
	{
		return;
	}

	AHouse* BestHouse = FindNearestUnvisitedHouse();

	if (BestHouse)
	{
		BlackboardComp->SetValueAsObject(TargetHouseKey, BestHouse);
		BlackboardComp->SetValueAsVector(TargetHouseLocationKey, BestHouse->GetActorLocation());
		BlackboardComp->SetValueAsBool(HasUnvisitedHouseKey, true);
	}
	else
	{
		BlackboardComp->ClearValue(TargetHouseKey);
		BlackboardComp->ClearValue(TargetHouseLocationKey);
		BlackboardComp->SetValueAsBool(HasUnvisitedHouseKey, false);
	}
}

ABaseItem* UStudentPerceptorWoutWenseleers::FindBestKnownItem() const
{
	const AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		return nullptr;
	}

	ABaseItem* BestItem = nullptr;
	int32 BestPriority = -1;
	float BestDistSq = TNumericLimits<float>::Max();

	for (ABaseItem* Item : KnownItems)
	{
		if (!IsValid(Item) || Item->GetItemType() == EItemType::Garbage)
		{
			continue;
		}

		const int32 Priority = GetItemPriority(Item);

		if (Priority <= 0)
		{
			continue;
		}
		
		const float DistSq = FVector::DistSquared(
			OwnerActor->GetActorLocation(),
			Item->GetActorLocation()
		);

		if (!BestItem || Priority > BestPriority ||
			(Priority == BestPriority && DistSq < BestDistSq))
		{
			BestItem = Item;
			BestPriority = Priority;
			BestDistSq = DistSq;
		}
	}

	return BestItem;
}

AHouse* UStudentPerceptorWoutWenseleers::FindNearestUnvisitedHouse() const
{
	const AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		return nullptr;
	}

	AHouse* BestHouse = nullptr;
	float BestDistSq = TNumericLimits<float>::Max();

	for (AHouse* House : KnownUnvisitedHouses)
	{
		if (!IsValid(House))
		{
			continue;
		}

		const float DistSq = FVector::DistSquared(
			OwnerActor->GetActorLocation(),
			House->GetActorLocation()
		);

		if (!BestHouse || DistSq < BestDistSq)
		{
			BestHouse = House;
			BestDistSq = DistSq;
		}
	}

	return BestHouse;
}

bool UStudentPerceptorWoutWenseleers::IsUsefulItem(const ABaseItem* Item) const
{
	return IsValid(Item) && GetItemPriority(Item) > 0;
}

void UStudentPerceptorWoutWenseleers::NotifyItemCollected(ABaseItem* Item)
{
	if (!Item)
	{
		return;
	}

	KnownItems.Remove(Item);
	ClearActorKeyIfMatching(TargetItemKey, Item);

	if (BlackboardComp)
	{
		BlackboardComp->ClearValue(TargetItemKey);
		BlackboardComp->ClearValue(TargetItemLocationKey);
		BlackboardComp->SetValueAsBool(HasItemKey, false);
	}

	RefreshItemTarget();
}

void UStudentPerceptorWoutWenseleers::RefreshInventoryAndMemory()
{
	ASurvivorPawn* Survivor = Cast<ASurvivorPawn>(AIController ? AIController->GetPawn() : nullptr);
	if (!Survivor)
	{
		return;
	}

	UInventoryComponent* Inventory = Survivor->GetComponentByClass<UInventoryComponent>();
	if (!Inventory)
	{
		return;
	}

	TArray<ABaseItem*> InventoryItems = Inventory->GetInventory();

	for (int32 SlotIdx = 0; SlotIdx < InventoryItems.Num(); ++SlotIdx)
	{
		ABaseItem* Item = InventoryItems[SlotIdx];

		if (!Item)
		{
			continue;
		}

		if (Item->GetValue() <= 0)
		{
			Inventory->RemoveItem(SlotIdx);
		}
	}
	RefreshItemTarget();
}

void UStudentPerceptorWoutWenseleers::RefreshSurvivalState()
{
	ASurvivorPawn* Survivor =
		Cast<ASurvivorPawn>(AIController->GetPawn());

	UStaminaComponent* StaminaComp = Survivor->GetComponentByClass<UStaminaComponent>();
	
	UHealthComponent* HealthComp = Survivor->GetComponentByClass<UHealthComponent>();

	if (!Survivor || !StaminaComp || !HealthComp)
	{
		return;
	}

	BlackboardComp->SetValueAsBool(
		ShouldEatKey,
		StaminaComp->GetCurrentStamina() < 5.f
	);

	BlackboardComp->SetValueAsBool(
		ShouldHealKey,
		HealthComp->GetHealth() < 5.f
	);
}