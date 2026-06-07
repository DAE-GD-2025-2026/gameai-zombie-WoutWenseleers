#include "BTTask_UseMedkitWoutWenseleers.h"

#include "AIController.h"

#include "Survivor/SurvivorPawn.h"
#include "Common/InventoryComponent.h"
#include "Items/BaseItem.h"
#include "Items/ItemType.h"
#include "StudentPerceptorWoutWenseleers.h"

UBTTask_UseMedkitWoutWenseleers::UBTTask_UseMedkitWoutWenseleers()
{
	NodeName = TEXT("Use Medkit");
}

EBTNodeResult::Type UBTTask_UseMedkitWoutWenseleers::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();

	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	ASurvivorPawn* Survivor =
		Cast<ASurvivorPawn>(AIController->GetPawn());

	if (!Survivor)
	{
		return EBTNodeResult::Failed;
	}

	UInventoryComponent* Inventory =
		Survivor->GetComponentByClass<UInventoryComponent>();

	if (!Inventory)
	{
		return EBTNodeResult::Failed;
	}

	const TArray<ABaseItem*>& Items =
		Inventory->GetInventory();

	for (int32 SlotIdx = 0; SlotIdx < Items.Num(); ++SlotIdx)
	{
		ABaseItem* Item = Items[SlotIdx];

		if (!Item)
		{
			continue;
		}

		if (Item->GetItemType() == EItemType::Medkit)
		{
			if (Inventory->UseItem(SlotIdx))
			{
				if (UStudentPerceptorWoutWenseleers* Perceptor =
					Survivor->FindComponentByClass<UStudentPerceptorWoutWenseleers>())
				{
					Perceptor->RefreshItemTarget();
					Perceptor->RefreshInventoryAndMemory();
				}

				return EBTNodeResult::Succeeded;
			}
		}
	}

	return EBTNodeResult::Failed;
}