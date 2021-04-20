// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_PredictLocation.h"

#include "GuardAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_PredictLocation::UBTTask_PredictLocation()
{
	NodeName = "Predict Location";
}

EBTNodeResult::Type UBTTask_PredictLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	APawn* Player = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("Player"));
	if (Player == nullptr)
	{
		return EBTNodeResult::Failed;
	}
	const FVector PredictedLocation = Player->GetActorLocation() + Player->GetVelocity() * 100.0f;
	OwnerComp.GetBlackboardComponent()->SetValueAsVector("PredictedPlayerLocation", PredictedLocation);
	return EBTNodeResult::Succeeded;
}
