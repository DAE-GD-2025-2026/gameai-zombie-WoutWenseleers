#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SearchHouseWoutWenseleers.generated.h"

class AHouse;

UCLASS()
class WENSELEERSWOUTZOMBIERUNTIME_API UBTTask_SearchHouseWoutWenseleers : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_SearchHouseWoutWenseleers();

protected:
	virtual EBTNodeResult::Type ExecuteTask(
		UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory
	) override;

	virtual void TickTask(
		UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory,
		float DeltaSeconds
	) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetHouseKey;

	UPROPERTY(EditAnywhere, Category = "Search")
	float SearchDuration = 2.f;

	UPROPERTY(EditAnywhere, Category = "Search")
	float TurnSpeedDegreesPerSecond = 240.0f;

private:
	float ElapsedSearchTime = 0.0f;
};