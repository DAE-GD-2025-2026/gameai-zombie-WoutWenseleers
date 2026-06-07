#include "BTTask_PickupTargetItemWoutWenseleers.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Survivor/SurvivorPawn.h"
#include "Common/InventoryComponent.h"
#include "Items/BaseItem.h"
#include "StudentPerceptorWoutWenseleers.h"

UBTTask_PickupTargetItemWoutWenseleers::UBTTask_PickupTargetItemWoutWenseleers()
{
	NodeName = TEXT("Pick Up Target Item");
}

EBTNodeResult::Type UBTTask_PickupTargetItemWoutWenseleers::ExecuteTask(
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
	ABaseItem* Item = Cast<ABaseItem>(
		Blackboard->GetValueAsObject(TargetItemKey.SelectedKeyName)
	);

	if (!Survivor || !Item)
	{
		return EBTNodeResult::Failed;
	}

	UInventoryComponent* Inventory = Survivor->GetComponentByClass<UInventoryComponent>();
	if (!Inventory)
	{
		return EBTNodeResult::Failed;
	}

	const float Distance = FVector::Dist(
		Survivor->GetActorLocation(),
		Item->GetActorLocation()
	);

	if (Distance > Inventory->GetPickupRange() + ExtraPickupTolerance)
	{
		return EBTNodeResult::Failed;
	}

	for (int32 SlotIdx = 0; SlotIdx < Inventory->GetInventoryCapacity(); ++SlotIdx)
	{
		if (Inventory->GrabItem(SlotIdx, Item))
		{
			if (UStudentPerceptorWoutWenseleers* Perceptor = Survivor->FindComponentByClass<UStudentPerceptorWoutWenseleers>())
			{
				Perceptor->NotifyItemCollected(Item);
			}
			else
			{
				Blackboard->ClearValue(TargetItemKey.SelectedKeyName);
			}
			
			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;
}