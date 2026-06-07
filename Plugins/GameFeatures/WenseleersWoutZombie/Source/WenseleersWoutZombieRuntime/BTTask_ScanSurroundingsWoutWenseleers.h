#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ScanSurroundingsWoutWenseleers.generated.h"

UCLASS()
class WENSELEERSWOUTZOMBIERUNTIME_API UBTTask_ScanSurroundingsWoutWenseleers
	: public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ScanSurroundingsWoutWenseleers();

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

	UPROPERTY(EditAnywhere)
	float ScanDuration = 1.0f;

private:
	float ElapsedTime = 0.0f;
	float StartYaw = 0.0f;
};