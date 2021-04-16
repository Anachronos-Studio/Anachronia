// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_FindClosestPatrolPoint.generated.h"

/**
 * 
 */
UCLASS()
class ANACHRONIA_API UBTService_FindClosestPatrolPoint : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_FindClosestPatrolPoint();

protected:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
