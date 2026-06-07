#include "BTTask_UseWeaponOnZombieWoutWenseleers.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Survivor/SurvivorPawn.h"
#include "Common/InventoryComponent.h"
#include "Items/BaseItem.h"
#include "Items/ItemType.h"
#include "Zombies/BaseZombie.h"
#include "StudentPerceptorWoutWenseleers.h"

UBTTask_UseWeaponOnZombieWoutWenseleers::UBTTask_UseWeaponOnZombieWoutWenseleers()
{
	NodeName = TEXT("Use Weapon On Zombie");
}

EBTNodeResult::Type UBTTask_UseWeaponOnZombieWoutWenseleers::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory
)
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	AAIController* AIController = OwnerComp.GetAIOwner();

	if (!Blackboard || !AIController)
	{
		return EBTNodeResult::Failed;
	}

	ASurvivorPawn* Survivor = Cast<ASurvivorPawn>(AIController->GetPawn());
	ABaseZombie* Zombie = Cast<ABaseZombie>(
		Blackboard->GetValueAsObject(TargetZombieKey.SelectedKeyName)
	);

	if (!Survivor || !Zombie)
	{
		return EBTNodeResult::Failed;
	}

	UInventoryComponent* Inventory = Survivor->GetComponentByClass<UInventoryComponent>();
	if (!Inventory)
	{
		return EBTNodeResult::Failed;
	}

	FVector Direction = Zombie->GetActorLocation() - Survivor->GetActorLocation();
	Direction.Z = 0.0f;

	if (!Direction.IsNearlyZero())
	{
		Survivor->SetActorRotation(Direction.Rotation());
		AIController->SetFocus(Zombie);
	}

	const TArray<ABaseItem*>& Items = Inventory->GetInventory();

	for (int32 SlotIdx = 0; SlotIdx < Items.Num(); ++SlotIdx)
	{
		ABaseItem* Item = Items[SlotIdx];

		if (!Item || Item->GetValue() <= 0)
		{
			continue;
		}

		const EItemType ItemType = Item->GetItemType();

		if (ItemType == EItemType::Shotgun || ItemType == EItemType::Pistol)
		{
			if (Inventory->UseItem(SlotIdx))
			{
				UStudentPerceptorWoutWenseleers* Perceptor = AIController->GetPawn()->FindComponentByClass<UStudentPerceptorWoutWenseleers>();

				Perceptor->RefreshInventoryAndMemory();
				return EBTNodeResult::Succeeded;
			}
		}
	}

	return EBTNodeResult::Failed;
}