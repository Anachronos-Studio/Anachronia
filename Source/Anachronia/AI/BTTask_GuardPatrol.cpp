// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_GuardPatrol.h"

#include "GuardPawnMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_GuardPatrol::UBTTask_GuardPatrol()
{
	NodeName = "Patrol";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_GuardPatrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	UGuardPawnMovementComponent* GuardMovement = GetGuardMovement(OwnerComp);
	GuardMovement->StartFollowingPath();
	if (GuardMovement->GetPathFollowState() == EGuardPathFollowState::Following)
	{
		return EBTNodeResult::InProgress;
	}
	else
	{
		UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
		Blackboard->SetValueAsVector("PatrolReturnLocation", GuardMovement->GetPathReturnToLocation());
		return EBTNodeResult::Failed;
	}
}

EBTNodeResult::Type UBTTask_GuardPatrol::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	GetGuardMovement(OwnerComp)->StopFollowingPath();

	return EBTNodeResult::Aborted;
}

void UBTTask_GuardPatrol::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UGuardPawnMovementComponent* GuardMovement = GetGuardMovement(OwnerComp);
	const EGuardPathFollowState GuardState = GuardMovement->GetPathFollowState();
	if (GuardState != EGuardPathFollowState::Following)
	{
		UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
		Blackboard->SetValueAsVector("PatrolReturnLocation", GuardMovement->GetPathReturnToLocation());
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
}
