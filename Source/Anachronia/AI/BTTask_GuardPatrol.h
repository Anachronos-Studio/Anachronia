// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BTTask_GuardBase.h"
#include "BTTask_GuardPatrol.generated.h"

/**
 * 
 */
UCLASS()
class ANACHRONIA_API UBTTask_GuardPatrol : public UBTTask_GuardBase
{
	GENERATED_BODY()

public:
	UBTTask_GuardPatrol();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
