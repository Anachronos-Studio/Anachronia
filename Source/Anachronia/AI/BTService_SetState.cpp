// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_SetState.h"

#include "GuardAIController.h"

UBTService_SetState::UBTService_SetState()
{
	NodeName = "Set State";
	State = EGuardState::Patrol;
	bNotifyBecomeRelevant = true;
}

void UBTService_SetState::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Cast<AGuardAIController>(OwnerComp.GetAIOwner())->SetState(State);

}

FString UBTService_SetState::GetStaticDescription() const
{
	return FString::Printf(TEXT("Set state: %s"), *StaticEnum<EGuardState>()->GetValueAsString(State));
}
