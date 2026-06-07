#include "BTTask_FindPatrolLocationWoutWenseleers.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

UBTTask_FindPatrolLocationWoutWenseleers::UBTTask_FindPatrolLocationWoutWenseleers()
{
	NodeName = TEXT("Find Patrol Location");
}

EBTNodeResult::Type UBTTask_FindPatrolLocationWoutWenseleers::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory
)
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	AAIController* AIController = OwnerComp.GetAIOwner();

	if (!Blackboard || !AIController || !AIController->GetPawn())
	{
		return EBTNodeResult::Failed;
	}

	APawn* Pawn = AIController->GetPawn();

	UNavigationSystemV1* NavSystem =
		FNavigationSystem::GetCurrent<UNavigationSystemV1>(AIController->GetWorld());

	if (!NavSystem)
	{
		return EBTNodeResult::Failed;
	}

	if (!bHasTargetYaw)
	{
		TargetYawDegrees = Pawn->GetActorRotation().Yaw;
		bHasTargetYaw = true;
	}

	TargetYawDegrees += FMath::RandRange(
		-DirectionJitterDegrees,
		DirectionJitterDegrees
	);

	const FVector ForwardDirection =
		FRotationMatrix(FRotator(0.0f, TargetYawDegrees, 0.0f)).GetUnitAxis(EAxis::X);

	const FVector DesiredLocation =
		Pawn->GetActorLocation() + ForwardDirection * ForwardDistance;

	FNavLocation ProjectedLocation;

	const bool bProjected = NavSystem->ProjectPointToNavigation(
		DesiredLocation,
		ProjectedLocation,
		FVector(SearchRadius, SearchRadius, 500.0f)
	);

	if (!bProjected)
	{
		return EBTNodeResult::Failed;
	}

	Blackboard->SetValueAsVector(
		PatrolLocationKey.SelectedKeyName,
		ProjectedLocation.Location
	);

	return EBTNodeResult::Succeeded;
}