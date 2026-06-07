#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BlackboardData.h"
#include "BTTask_UseWeaponOnZombieWoutWenseleers.generated.h"

UCLASS()
class WENSELEERSWOUTZOMBIERUNTIME_API UBTTask_UseWeaponOnZombieWoutWenseleers : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_UseWeaponOnZombieWoutWenseleers();

protected:
	virtual EBTNodeResult::Type ExecuteTask(
		UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory
	) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetZombieKey;
};