// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ChooseNewPatrolPath.h"

#include "GuardAIController.h"

UBTTask_ChooseNewPatrolPath::UBTTask_ChooseNewPatrolPath()
{
	NodeName = "Choose new patrol path";
}

EBTNodeResult::Type UBTTask_ChooseNewPatrolPath::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	GetGuardController(OwnerComp)->ChooseNewPatrolPath();
	
	return EBTNodeResult::Succeeded;
}
