#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BlackboardData.h"
#include "BTTask_FindPatrolLocationWoutWenseleers.generated.h"

UCLASS()
class WENSELEERSWOUTZOMBIERUNTIME_API UBTTask_FindPatrolLocationWoutWenseleers : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FindPatrolLocationWoutWenseleers();

protected:
	virtual EBTNodeResult::Type ExecuteTask(
		UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory
	) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector PatrolLocationKey;

	UPROPERTY(EditAnywhere, Category = "Navigation")
	float SearchRadius = 1200.0f;
private:
	bool bHasTargetYaw = false;
	float TargetYawDegrees = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Navigation")
	float DirectionJitterDegrees = 20.0f;

	UPROPERTY(EditAnywhere, Category = "Navigation")
	float ForwardDistance = 900.0f;
};