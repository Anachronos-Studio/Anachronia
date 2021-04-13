// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_GuardPatrol.h"

#include "GuardPawn.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_GuardPatrol::UBTTask_GuardPatrol()
{
	NodeName = "Patrol";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_GuardPatrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AGuardPawn* Guard = GetGuardPawn(OwnerComp);
	Guard->StartFollowingPath();
	if (Guard->GetPathFollowState() == EGuardPathFollowState::Following)
	{
		return EBTNodeResult::InProgress;
	}
	else
	{
		UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
		Blackboard->SetValueAsVector("PatrolReturnLocation", Guard->GetPathReturnToLocation());
		return EBTNodeResult::Failed;
	}
}

EBTNodeResult::Type UBTTask_GuardPatrol::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	GetGuardPawn(OwnerComp)->StopFollowingPath();

	return EBTNodeResult::Aborted;
}

void UBTTask_GuardPatrol::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AGuardPawn* Guard = GetGuardPawn(OwnerComp);
	const EGuardPathFollowState GuardState = Guard->GetPathFollowState();
	if (GuardState != EGuardPathFollowState::Following)
	{
		UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
		Blackboard->SetValueAsVector("PatrolReturnLocation", Guard->GetPathReturnToLocation());
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
}
