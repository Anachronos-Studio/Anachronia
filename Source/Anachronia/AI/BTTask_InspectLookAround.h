// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BTTask_GuardBase.h"
#include "BTTask_InspectLookAround.generated.h"

struct FBTInspectLookTaskMemory
{
	FRotator OriginalRotation;
	float Timer;
	int8 Counter;
};

/**
 * 
 */
UCLASS()
class ANACHRONIA_API UBTTask_InspectLookAround : public UBTTask_GuardBase
{
	GENERATED_BODY()

public:
	UBTTask_InspectLookAround();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
public:
	virtual uint16 GetInstanceMemorySize() const override;
private:
	void TurnToLook(UBehaviorTreeComponent& OwnerComp, FBTInspectLookTaskMemory* MyMemory) const;
};
