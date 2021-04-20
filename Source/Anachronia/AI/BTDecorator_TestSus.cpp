#include "BTDecorator_TestSus.h"
#include "GuardAIController.h"

UBTDecorator_TestSus::UBTDecorator_TestSus()
{
	NodeName = "Sus?";
	bNotifyTick = true;
}

bool UBTDecorator_TestSus::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AGuardAIController* Guard = Cast<AGuardAIController>(OwnerComp.GetAIOwner());

	return Guard->IsSusEnough(Level);
}

void UBTDecorator_TestSus::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	ConditionalFlowAbort(OwnerComp, EBTDecoratorAbortRequest::ConditionResultChanged);
}

FString UBTDecorator_TestSus::GetStaticDescription() const
{
	const FString LevelStr = UEnum::GetValueAsString<ESusLevel>(Level);
	const FString Desc = FString::Printf(TEXT("Sus >= %s?"), *LevelStr);
	return FString::Printf(TEXT("%s: %s"), *Super::GetStaticDescription(), *Desc);
}
