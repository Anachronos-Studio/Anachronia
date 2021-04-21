// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_SetState.generated.h"

enum class EGuardState : uint8;

/**
 * 
 */
UCLASS(HideCategories = ("Service"))
class ANACHRONIA_API UBTService_SetState : public UBTService
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	EGuardState State;

	UBTService_SetState();
	
protected:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
};
