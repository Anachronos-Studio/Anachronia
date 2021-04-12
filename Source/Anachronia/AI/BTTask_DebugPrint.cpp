#include "BTTask_DebugPrint.h"

UBTTask_DebugPrint::UBTTask_DebugPrint()
{
	NodeName = "DebugPrint";
	MessageColor = FColor::White;
}

EBTNodeResult::Type UBTTask_DebugPrint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	GEngine->AddOnScreenDebugMessage
	(-1, MessageDuration, MessageColor, FString::Printf(TEXT("%s AI: %s"), *OwnerComp.GetOwner()->GetName(), *NodeName));
	UE_LOG(LogTemp, Display, TEXT("%s AI: %s"), *OwnerComp.GetOwner()->GetName(), *NodeName);

	return EBTNodeResult::Succeeded;
}