// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_GuardBase.generated.h"

class UGuardPawnMovementComponent;
class AGuardPawn;
class AGuardAIController;

/**
 *
 */
UCLASS(Abstract)
class ANACHRONIA_API UBTTask_GuardBase : public UBTTaskNode
{
	GENERATED_BODY()

public:
	AGuardAIController* GetGuardController(UBehaviorTreeComponent& OwnerComp) const;
	AGuardPawn* GetGuardPawn(UBehaviorTreeComponent& OwnerComp) const;
};
