#include "BTTask_FindFleeLocationWoutWenseleers.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

#include "Zombies/BaseZombie.h"

UBTTask_FindFleeLocationWoutWenseleers::UBTTask_FindFleeLocationWoutWenseleers()
{
	NodeName = TEXT("Find Flee Location");
}

EBTNodeResult::Type UBTTask_FindFleeLocationWoutWenseleers::ExecuteTask(
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

	ABaseZombie* Zombie = Cast<ABaseZombie>(
		Blackboard->GetValueAsObject(TargetZombieKey.SelectedKeyName)
	);

	if (!Zombie)
	{
		return EBTNodeResult::Failed;
	}

	UNavigationSystemV1* NavSystem =
		FNavigationSystem::GetCurrent<UNavigationSystemV1>(AIController->GetWorld());

	if (!NavSystem)
	{
		return EBTNodeResult::Failed;
	}

	FVector FleeDirection = Pawn->GetActorLocation() - Zombie->GetActorLocation();
	FleeDirection.Z = 0.0f;

	if (FleeDirection.IsNearlyZero())
	{
		FleeDirection = -Pawn->GetActorForwardVector();
		FleeDirection.Z = 0.0f;
	}

	FleeDirection.Normalize();

	const FVector DesiredFleeLocation =
		Pawn->GetActorLocation() + FleeDirection * FleeDistance;

	FNavLocation ProjectedFleeLocation;

	const bool bProjected = NavSystem->ProjectPointToNavigation(
		DesiredFleeLocation,
		ProjectedFleeLocation,
		FVector(ProjectionExtent, ProjectionExtent, 500.0f)
	);

	if (!bProjected)
	{
		return EBTNodeResult::Failed;
	}

	Blackboard->SetValueAsVector(
		FleeLocationKey.SelectedKeyName,
		ProjectedFleeLocation.Location
	);

	return EBTNodeResult::Succeeded;
}