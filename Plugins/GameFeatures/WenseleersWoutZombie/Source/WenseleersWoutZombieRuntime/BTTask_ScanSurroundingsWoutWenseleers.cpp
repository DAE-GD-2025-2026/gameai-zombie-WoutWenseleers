#include "BTTask_ScanSurroundingsWoutWenseleers.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

#include "Zombies/BaseZombie.h"

UBTTask_ScanSurroundingsWoutWenseleers::UBTTask_ScanSurroundingsWoutWenseleers()
{
	NodeName = TEXT("Scan Surroundings");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_ScanSurroundingsWoutWenseleers::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory)
{
	ElapsedTime = 0.0f;

	APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();

	if (!Pawn)
	{
		return EBTNodeResult::Failed;
	}

	StartYaw = Pawn->GetActorRotation().Yaw;

	return EBTNodeResult::InProgress;
}

void UBTTask_ScanSurroundingsWoutWenseleers::TickTask(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory,
	float DeltaSeconds)
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	if (Blackboard && Blackboard->GetValueAsObject(TEXT("TargetZombie")))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}
	
	APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();

	if (!Pawn)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	ElapsedTime += DeltaSeconds;

	const float Alpha =
		FMath::Clamp(ElapsedTime / ScanDuration, 0.f, 1.f);

	FRotator Rot = Pawn->GetActorRotation();
	Rot.Yaw = StartYaw + Alpha * 360.f;

	Pawn->SetActorRotation(Rot);

	if (Alpha >= 1.f)
	{
		FinishLatentTask(
			OwnerComp,
			EBTNodeResult::Succeeded
		);
	}
}