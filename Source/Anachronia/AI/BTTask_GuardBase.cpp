// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_GuardBase.h"
#include "GuardAIController.h"
#include "GuardPawn.h"

AGuardAIController* UBTTask_GuardBase::GetGuardController(UBehaviorTreeComponent& OwnerComp) const
{
	return Cast<AGuardAIController>(OwnerComp.GetOwner());
}

AGuardPawn* UBTTask_GuardBase::GetGuardPawn(UBehaviorTreeComponent& OwnerComp) const
{
	return GetGuardController(OwnerComp)->GetGuardPawn();
}

UGuardPawnMovementComponent* UBTTask_GuardBase::GetGuardMovement(UBehaviorTreeComponent& OwnerComp) const
{
	return GetGuardPawn(OwnerComp)->GetGuardMovement();
}
