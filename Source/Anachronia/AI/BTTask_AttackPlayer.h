// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BTTask_GuardBase.h"
#include "BTTask_AttackPlayer.generated.h"

/**
 * 
 */
UCLASS()
class ANACHRONIA_API UBTTask_AttackPlayer : public UBTTask_GuardBase
{
	GENERATED_BODY()

public:
	UBTTask_AttackPlayer();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
};
