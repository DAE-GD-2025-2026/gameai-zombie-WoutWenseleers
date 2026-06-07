#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "StudentPerceptorWoutWenseleers.generated.h"

class AAIController;
class UBlackboardComponent;
class ABaseItem;
class AHouse;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class WENSELEERSWOUTZOMBIERUNTIME_API UStudentPerceptorWoutWenseleers : public UActorComponent
{
	GENERATED_BODY()

public:
	UStudentPerceptorWoutWenseleers();

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName TargetZombieKey = TEXT("TargetZombie");

	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName LastKnownZombieLocationKey = TEXT("LastKnownZombieLocation");

	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName HasZombieKey = TEXT("HasZombie");

	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName TargetItemKey = TEXT("TargetItem");

	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName TargetItemLocationKey = TEXT("TargetItemLocation");

	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName HasItemKey = TEXT("HasItem");

	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName TargetHouseKey = TEXT("TargetHouse");

	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName TargetHouseLocationKey = TEXT("TargetHouseLocation");

	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName HasUnvisitedHouseKey = TEXT("HasUnvisitedHouse");

	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName IsSearchingHouseKey = TEXT("IsSearchingHouse");

	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName ShouldFleeCombatKey = TEXT("ShouldFleeCombat");

	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName ShouldEatKey = TEXT("ShouldEat");
	
	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName ShouldHealKey = TEXT("ShouldHeal");
	
	void MarkHouseVisited(AHouse* House);
	void ForgetKnownItem(ABaseItem* Item);
	void RefreshItemTarget();
	void NotifyItemCollected(ABaseItem* Item);
	void RefreshInventoryAndMemory();
	void RefreshSurvivalState();
	
private:
	void HandleZombiePerception(AActor* Actor, bool bSuccessfullySensed);
	void HandleItemPerception(ABaseItem* Item, bool bSuccessfullySensed);
	void HandleHousePerception(AHouse* House, bool bSuccessfullySensed);

	void RegisterKnownItem(ABaseItem* Item);
	void RegisterKnownHouse(AHouse* House);

	int32 GetItemPriority(const ABaseItem* Item) const;

	void RefreshHouseTarget();

	bool IsUsefulItem(const ABaseItem* Item) const;
	ABaseItem* FindBestKnownItem() const;
	AHouse* FindNearestUnvisitedHouse() const;
	
	void ClearActorKeyIfMatching(FName KeyName, AActor* Actor) const;

	UPROPERTY()
	TObjectPtr<AAIController> AIController = nullptr;

	UPROPERTY()
	TObjectPtr<UBlackboardComponent> BlackboardComp = nullptr;

	UPROPERTY()
	TArray<TObjectPtr<ABaseItem>> KnownItems;
	UPROPERTY()
	TArray<TObjectPtr<AHouse>> KnownUnvisitedHouses;
	UPROPERTY()
	TArray<TObjectPtr<AHouse>> KnownHouses;
};