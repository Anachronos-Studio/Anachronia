// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_CallForBackup.generated.h"

/**
 * 
 */
UCLASS()
class ANACHRONIA_API UBTService_CallForBackup : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_CallForBackup();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
public:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	float SoundMaxRange;
};
