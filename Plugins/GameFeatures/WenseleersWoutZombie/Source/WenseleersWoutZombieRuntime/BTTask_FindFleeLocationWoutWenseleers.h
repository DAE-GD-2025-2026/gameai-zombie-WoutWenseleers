#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindFleeLocationWoutWenseleers.generated.h"

UCLASS()
class WENSELEERSWOUTZOMBIERUNTIME_API UBTTask_FindFleeLocationWoutWenseleers : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FindFleeLocationWoutWenseleers();

protected:
	virtual EBTNodeResult::Type ExecuteTask(
		UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory
	) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetZombieKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector FleeLocationKey;

	UPROPERTY(EditAnywhere, Category = "Flee")
	float FleeDistance = 900.0f;

	UPROPERTY(EditAnywhere, Category = "Flee")
	float ProjectionExtent = 600.0f;
};