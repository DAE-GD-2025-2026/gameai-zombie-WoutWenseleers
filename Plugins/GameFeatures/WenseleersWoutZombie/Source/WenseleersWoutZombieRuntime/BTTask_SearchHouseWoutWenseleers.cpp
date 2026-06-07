#include "BTTask_SearchHouseWoutWenseleers.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "StudentPerceptorWoutWenseleers.h"
#include "Village/House/House.h"

UBTTask_SearchHouseWoutWenseleers::UBTTask_SearchHouseWoutWenseleers()
{
	NodeName = TEXT("Search House");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_SearchHouseWoutWenseleers::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory
)
{
	ElapsedSearchTime = 0.0f;

	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	if (!AIController || !Blackboard || !AIController->GetPawn())
	{
		return EBTNodeResult::Failed;
	}

	AHouse* House = Cast<AHouse>(
		Blackboard->GetValueAsObject(TargetHouseKey.SelectedKeyName)
	);

	if (!House)
	{
		return EBTNodeResult::Failed;
	}

	AIController->StopMovement();

	return EBTNodeResult::InProgress;
}

void UBTTask_SearchHouseWoutWenseleers::TickTask(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory,
	float DeltaSeconds
)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	if (!AIController || !Blackboard || !AIController->GetPawn())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	APawn* Pawn = AIController->GetPawn();

	FRotator NewRotation = Pawn->GetActorRotation();
	NewRotation.Yaw += TurnSpeedDegreesPerSecond * DeltaSeconds;
	Pawn->SetActorRotation(NewRotation);

	ElapsedSearchTime += DeltaSeconds;

	if (ElapsedSearchTime < SearchDuration)
	{
		return;
	}

	AHouse* House = Cast<AHouse>(
		Blackboard->GetValueAsObject(TargetHouseKey.SelectedKeyName)
	);

	UStudentPerceptorWoutWenseleers* Perceptor = Pawn->FindComponentByClass<UStudentPerceptorWoutWenseleers>();

	if (House && Perceptor)
	{
		Perceptor->MarkHouseVisited(House);
	}

	Blackboard->ClearValue(TargetHouseKey.SelectedKeyName);

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}