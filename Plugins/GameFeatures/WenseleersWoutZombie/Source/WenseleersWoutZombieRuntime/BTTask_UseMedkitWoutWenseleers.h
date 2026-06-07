#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_UseMedkitWoutWenseleers.generated.h"

UCLASS()
class WENSELEERSWOUTZOMBIERUNTIME_API UBTTask_UseMedkitWoutWenseleers : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_UseMedkitWoutWenseleers();

protected:
	virtual EBTNodeResult::Type ExecuteTask(
		UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory
	) override;
};