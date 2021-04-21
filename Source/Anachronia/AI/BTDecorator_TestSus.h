// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GuardAIController.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_TestSus.generated.h"
/**
 * 
 */
UCLASS()
class ANACHRONIA_API UBTDecorator_TestSus : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_TestSus();

protected:
	UPROPERTY(EditAnywhere)
	ESusLevel Level;
	
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
public:
	virtual FString GetStaticDescription() const override;
};
