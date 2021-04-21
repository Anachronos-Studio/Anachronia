// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_SetAlertness.h"

#include "GuardAIController.h"

UBTService_SetAlertness::UBTService_SetAlertness()
{
	NodeName = "Set Alertness";
	Alertness = EAlertness::Neutral;
	bNotifyBecomeRelevant = true;
}

void UBTService_SetAlertness::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Cast<AGuardAIController>(OwnerComp.GetAIOwner())->SetAlertness(Alertness);
}

FString UBTService_SetAlertness::GetStaticDescription() const
{
	return FString::Printf(TEXT("Set alertness: %s"), *StaticEnum<EAlertness>()->GetValueAsString(Alertness));
}
