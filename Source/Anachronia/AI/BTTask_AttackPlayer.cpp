// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_AttackPlayer.h"
#include "GuardPawn.h"
#include "Anachronia/PlayerCharacter/AnachroniaPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_AttackPlayer::UBTTask_AttackPlayer()
{
	NodeName = "Attack player";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_AttackPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAnachroniaPlayer* Player = Cast<AAnachroniaPlayer>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("Player"));
	AGuardPawn* Guard = GetGuardPawn(OwnerComp);
	Guard->OnAttack();

	return EBTNodeResult::Succeeded;
}

FString UBTTask_AttackPlayer::GetStaticDescription() const
{
	return Super::GetStaticDescription();
}
