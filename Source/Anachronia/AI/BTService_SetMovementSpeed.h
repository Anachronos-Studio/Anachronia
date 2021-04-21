// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_SetMovementSpeed.generated.h"

enum class EMovementSpeed : uint8;
/**
 * 
 */
UCLASS(HideCategories=("Service"))
class ANACHRONIA_API UBTService_SetMovementSpeed : public UBTService
{
	GENERATED_BODY()

public:
	UPROPERTY(Category = MovementSpeed, EditAnywhere)
	EMovementSpeed Speed;
	
	UBTService_SetMovementSpeed();
	
protected:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
public:
	virtual FString GetStaticDescription() const override;
};
