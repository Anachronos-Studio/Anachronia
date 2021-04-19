#include "BTDecorator_TestSus.h"
#include "GuardAIController.h"

UBTDecorator_TestSus::UBTDecorator_TestSus()
{
	NodeName = "Sus?";
}

bool UBTDecorator_TestSus::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AGuardAIController* Guard = Cast<AGuardAIController>(OwnerComp.GetAIOwner());

	return Guard->IsSusEnough(Level);
}

FString UBTDecorator_TestSus::GetStaticDescription() const
{
	const FString LevelStr = UEnum::GetValueAsString<ESusLevel>(Level);
	return FString::Printf(TEXT("Sus >= %s?"), *LevelStr);
}
