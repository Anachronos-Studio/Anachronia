// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BehaviorTree/BTService.h"
#include "BTService_SetPatrolling.generated.h"

/**
 * 
 */
UCLASS(HideCategories=("Service"))
class ANACHRONIA_API UBTService_SetPatrolling : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_SetPatrolling();

	UPROPERTY(Category = "Set patrolling", EditAnywhere)
	bool bPatrolling = false;

protected:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
public:
	virtual FString GetStaticDescription() const override;
};
