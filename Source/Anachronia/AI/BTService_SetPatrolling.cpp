// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_SetPatrolling.h"

#include "GuardAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_SetPatrolling::UBTService_SetPatrolling()
{
	NodeName = "Set patrolling";
	bNotifyBecomeRelevant = true;
	bNotifyTick = false;
}

void UBTService_SetPatrolling::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UE_LOG(LogTemp, Display, TEXT("I'm relevant!!"));
	AGuardAIController* Guard = Cast<AGuardAIController>(OwnerComp.GetAIOwner());
	check(Guard != nullptr);
	const bool bWasPatrollingBefore = Guard->GetBlackboardComponent()->GetValueAsBool("Patrolling");
	if (!bWasPatrollingBefore && bPatrolling)
	{
		Guard->FindClosestPatrolPoint();
	}

	Guard->GetBlackboardComponent()->SetValueAsBool("Patrolling", bPatrolling);
}

FString UBTService_SetPatrolling::GetStaticDescription() const
{
	if (bPatrolling)
	{
		return TEXT("On patrol.");
	}
	else
	{
		return TEXT("Away from patrol path.");
	}
}
