// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_SetAlertness.generated.h"

enum class EAlertness : uint8;
/**
 * 
 */
UCLASS(HideCategories=("Service"))
class ANACHRONIA_API UBTService_SetAlertness : public UBTService
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	EAlertness Alertness;
	
	UBTService_SetAlertness();
protected:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
public:
	virtual FString GetStaticDescription() const override;
};
