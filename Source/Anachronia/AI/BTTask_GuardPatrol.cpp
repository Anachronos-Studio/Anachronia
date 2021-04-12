// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_GuardPatrol.h"

#include "GuardPawn.h"

UBTTask_GuardPatrol::UBTTask_GuardPatrol()
{
	NodeName = "Patrol";
}

EBTNodeResult::Type UBTTask_GuardPatrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	GetGuardPawn(OwnerComp)->StartFollowingPath();

	return EBTNodeResult::InProgress;
}
