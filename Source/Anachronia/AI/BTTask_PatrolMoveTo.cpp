// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_PatrolMoveTo.h"

#include "GuardAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_PatrolMoveTo::UBTTask_PatrolMoveTo()
{
	NodeName = "Patrol Move";
	BlackboardKey.SelectedKeyName = "NavigationGoalLocation";
	bReachTestIncludesAgentRadius = false;
	bReachTestIncludesGoalRadius = false;
	bObserveBlackboardValue = true;
}

EBTNodeResult::Type UBTTask_PatrolMoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AGuardAIController* Guard = Cast<AGuardAIController>(OwnerComp.GetAIOwner());
	if (Guard->GetPatrolPath() == nullptr)
	{
		Guard->ResetRotation();
		return EBTNodeResult::Succeeded;
	}
	else
	{
		const FVector GoalLocation = Guard->GetCurrentPatrolGoal();
		OwnerComp.GetBlackboardComponent()->SetValueAsVector("NavigationGoalLocation", GoalLocation);
		return Super::ExecuteTask(OwnerComp, NodeMemory);
	}
}

void UBTTask_PatrolMoveTo::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AGuardAIController* Guard = Cast<AGuardAIController>(OwnerComp.GetAIOwner());
	APawn* Pawn = Guard->GetPawn();

	// If the guard is close to reaching their goal, but does not intend to stop at that point,
	// we move the goalpost to the next path point to avoid the awkward little stop the pawn otherwise does
	
	const FVector CurrentGoal = OwnerComp.GetBlackboardComponent()->GetValueAsVector("NavigationGoalLocation");
	const FVector PredictedNextLocation = Pawn->GetActorLocation() + Pawn->GetVelocity() * DeltaSeconds;
	FVector Delta = PredictedNextLocation - CurrentGoal;
	Delta.Z = 0;
	//UE_LOG(LogTemp, Display, TEXT("Distance: %f"), Delta.Size());
	if (Delta.IsNearlyZero(5.0f))
	{
		if (Guard->GetCurrentPatrolStopInfo() == nullptr)
		{
			//UE_LOG(LogTemp, Display, TEXT("Haha, moving goalpost fucker!!"));
			Guard->PickNextPatrolPoint();
			const FVector GoalLocation = Guard->GetCurrentPatrolGoal();
			OwnerComp.GetBlackboardComponent()->SetValueAsVector("NavigationGoalLocation", GoalLocation);
		}
	}
	
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
}

EBTNodeResult::Type UBTTask_PatrolMoveTo::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UE_LOG(LogTemp, Display, TEXT("Abort patrol!"));
	//GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Red, TEXT("Abort task!!!"));
	return Super::AbortTask(OwnerComp, NodeMemory);
}