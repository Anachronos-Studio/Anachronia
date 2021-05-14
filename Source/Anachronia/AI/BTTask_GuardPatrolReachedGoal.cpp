// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_GuardPatrolReachedGoal.h"

#include "GuardAIController.h"
#include "GuardPatrolPath.h"
#include "GuardPawn.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_GuardPatrolReachedGoal::UBTTask_GuardPatrolReachedGoal()
{
	NodeName = "Patrol move completed";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_GuardPatrolReachedGoal::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	
	AGuardAIController* Guard = GetGuardController(OwnerComp);
	
	const FVector CurrentGoal = OwnerComp.GetBlackboardComponent()->GetValueAsVector("NavigationGoalLocation");
	FVector Delta = Guard->GetPawn()->GetActorLocation() - CurrentGoal;
	Delta.Z = 0;
	if (Delta.IsNearlyZero(1.5f))
	{
		// Guard reached their goal, do they intend to stop and wait here?
		FBTPatrolTaskMemory* MyMemory = (FBTPatrolTaskMemory*)NodeMemory;
		
		if (Guard->GetPatrolPath() == nullptr)
		{
			// No need to constantly switch between moveto and wait tasks when no patrol path exists, just wait a fixed small amount instead of 0 seconds
			MyMemory->RemainingStopTime = 5.0f;
			return EBTNodeResult::InProgress;
		}
		
		FPatrolStop* Stop = Guard->GetCurrentPatrolStopInfo();
		if (Stop != nullptr)
		{
			Guard->PickNextPatrolPoint();
			MyMemory->RemainingStopTime = Stop->Duration;
			return EBTNodeResult::InProgress;
		}
	}

	Guard->PickNextPatrolPoint();
	return EBTNodeResult::Succeeded;
}

void UBTTask_GuardPatrolReachedGoal::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FBTPatrolTaskMemory* MyMemory = (FBTPatrolTaskMemory*)NodeMemory;
	MyMemory->RemainingStopTime -= DeltaSeconds;
	if (MyMemory->RemainingStopTime <= 0.0f)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

uint16 UBTTask_GuardPatrolReachedGoal::GetInstanceMemorySize() const
{
	return sizeof(FBTPatrolTaskMemory);
}

void UBTTask_GuardPatrolReachedGoal::DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const
{
	Super::DescribeRuntimeValues(OwnerComp, NodeMemory, Verbosity, Values);
	FBTPatrolTaskMemory* MyMemory = (FBTPatrolTaskMemory*)NodeMemory;
	if (MyMemory->RemainingStopTime > 0.0f)
	{
		Values.Add(FString::Printf(TEXT("at stop: %ss remains"), *FString::SanitizeFloat(MyMemory->RemainingStopTime)));
	}
}
