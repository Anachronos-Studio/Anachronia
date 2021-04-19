// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_FindClosestPatrolPoint.h"

#include "GuardAIController.h"

UBTService_FindClosestPatrolPoint::UBTService_FindClosestPatrolPoint()
{
	NodeName = "Find closest patrol point";
	bNotifyBecomeRelevant = true;
}

void UBTService_FindClosestPatrolPoint::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AGuardAIController* Guard = Cast<AGuardAIController>(OwnerComp.GetAIOwner());
	check(Guard != nullptr);
	UE_LOG(LogTemp, Display, TEXT("I'm relevant!!"));
	Guard->FindClosestPatrolPoint();
}
