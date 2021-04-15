// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BTTask_GuardBase.h"
#include "BTTask_GuardPatrol.generated.h"

struct FBTPatrolTaskMemory
{
	float RemainingStopTime;
};

/**
 * 
 */
UCLASS()
class ANACHRONIA_API UBTTask_GuardPatrol : public UBTTask_GuardBase
{
	GENERATED_BODY()

public:
	UBTTask_GuardPatrol();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
public:
	virtual uint16 GetInstanceMemorySize() const override;
	virtual void DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const override;
};
