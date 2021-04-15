// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_Patrol.h"

#include "GuardAIController.h"

UBTService_Patrol::UBTService_Patrol()
{
	bNotifyBecomeRelevant = true;
	NodeName = "Patrol";
}

void UBTService_Patrol::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AGuardAIController* Guard = Cast<AGuardAIController>(OwnerComp.GetAIOwner());
	const FVector GoalLocation = Guard->GetCurrentPatrolGoal();
	OwnerComp.GetBlackboardComponent()->SetValueAsVector("NavigationGoalLocation", GoalLocation);
}
