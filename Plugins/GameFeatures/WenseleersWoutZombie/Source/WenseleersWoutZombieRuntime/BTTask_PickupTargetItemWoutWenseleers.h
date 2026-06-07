#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BlackboardData.h"
#include "BTTask_PickupTargetItemWoutWenseleers.generated.h"

UCLASS()
class WENSELEERSWOUTZOMBIERUNTIME_API UBTTask_PickupTargetItemWoutWenseleers : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_PickupTargetItemWoutWenseleers();

protected:
	virtual EBTNodeResult::Type ExecuteTask(
		UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory
	) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetItemKey;

	UPROPERTY(EditAnywhere, Category = "Pickup")
	float ExtraPickupTolerance = 50.0f;
};