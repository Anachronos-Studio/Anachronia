#include "BTService_SetMovementSpeed.h"

#include "GuardAIController.h"

UBTService_SetMovementSpeed::UBTService_SetMovementSpeed()
{
	NodeName = "Set Movement Speed";
	bNotifyBecomeRelevant = true;
}

void UBTService_SetMovementSpeed::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Cast<AGuardAIController>(OwnerComp.GetAIOwner())->SetMovementSpeed(Speed);
}

FString UBTService_SetMovementSpeed::GetStaticDescription() const
{
	return FString::Printf(TEXT("Set Movement Speed: %s"), *StaticEnum<EMovementSpeed>()->GetValueAsString(Speed));
}
