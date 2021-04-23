#include "BTDecorator_CanAttackPlayer.h"
#include "GuardAIController.h"
#include "GuardPawn.h"
#include "Anachronia/PlayerCharacter/AnachroniaPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_CanAttackPlayer::UBTDecorator_CanAttackPlayer()
{
	NodeName = "Can attack player?";
	bNotifyTick = true;
}

bool UBTDecorator_CanAttackPlayer::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AAnachroniaPlayer* Player = Cast<AAnachroniaPlayer>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("Player"));
	AGuardPawn* Guard = Cast<AGuardPawn>(OwnerComp.GetAIOwner()->GetPawn());
	
	if (Player == nullptr)
	{
		return false;
	}

	const float Distance = FVector::Dist2D(Guard->GetActorLocation(), Player->GetActorLocation());
	const float ZDistance = FMath::Abs(Guard->GetActorLocation().Z - Player->GetActorLocation().Z);
	const float MaxZDistance = Guard->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	//UE_LOG(LogTemp, Display, TEXT("Dist: %f, z: %f, cool: %f"), Distance, ZDistance, AttackCooldownTimer);
	if (Distance < Guard->DistanceBeforeAttacking && ZDistance < MaxZDistance)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void UBTDecorator_CanAttackPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	ConditionalFlowAbort(OwnerComp, EBTDecoratorAbortRequest::ConditionResultChanged);
}

FString UBTDecorator_CanAttackPlayer::GetStaticDescription() const
{
	return Super::GetStaticDescription();
}
