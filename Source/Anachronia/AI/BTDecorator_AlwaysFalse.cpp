// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_AlwaysFalse.h"

UBTDecorator_AlwaysFalse::UBTDecorator_AlwaysFalse()
{
	
}

bool UBTDecorator_AlwaysFalse::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	return false;
}
