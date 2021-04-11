

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_DebugPrint.generated.h"

/**
 * 
 */
UCLASS()
class ANACHRONIA_API UBTTask_DebugPrint : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_DebugPrint();

	UPROPERTY(EditAnywhere)
	FColor MessageColor;

	UPROPERTY(EditAnywhere)
	float MessageDuration = 2.0f;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
