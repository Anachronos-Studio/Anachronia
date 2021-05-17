// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_InspectLookAround.h"

#include "GuardAIController.h"
#include "GuardPawn.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_InspectLookAround::UBTTask_InspectLookAround()
{
	NodeName = "Look around";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_InspectLookAround::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FBTInspectLookTaskMemory* MyMemory = (FBTInspectLookTaskMemory*)NodeMemory;
	MyMemory->Counter = 2;
	MyMemory->OriginalRotation = GetGuardController(OwnerComp)->GetControlRotation();
	TurnToLook(OwnerComp, MyMemory);
	UE_LOG(LogTemp, Display, TEXT("Execute look around"));
	if (OwnerComp.GetBlackboardComponent()->GetValueAsBool("SawBody"))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool("SawBody", false);
		GetGuardPawn(OwnerComp)->OnInspectDeadGuardBody();
	}
	return EBTNodeResult::InProgress;
}

void UBTTask_InspectLookAround::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FBTInspectLookTaskMemory* MyMemory = (FBTInspectLookTaskMemory*)NodeMemory;
	MyMemory->Timer -= DeltaSeconds;
	if (MyMemory->Timer <= 0.0f)
	{
		MyMemory->Counter--;
		if (MyMemory->Counter == 0)
		{
			GetGuardController(OwnerComp)->SetControlRotation(MyMemory->OriginalRotation);
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
		else
		{
			TurnToLook(OwnerComp, MyMemory);
		}
	}
}

uint16 UBTTask_InspectLookAround::GetInstanceMemorySize() const
{
	return sizeof(FBTInspectLookTaskMemory);
}

void UBTTask_InspectLookAround::TurnToLook(UBehaviorTreeComponent& OwnerComp, FBTInspectLookTaskMemory* MyMemory) const
{
	AGuardPawn* GuardPawn = GetGuardPawn(OwnerComp);
	MyMemory->Timer = GuardPawn->LookAroundWhenInspectingDuration;
	const int Dir = (MyMemory->Counter % 2 == 0) ? 1 : -1;
	const FRotator NewRotation = MyMemory->OriginalRotation + FRotator(0.0f, 80.0f * Dir, 0.0f);
	GetGuardController(OwnerComp)->SetControlRotation(NewRotation);
}
