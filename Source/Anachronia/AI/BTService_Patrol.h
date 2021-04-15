#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_Patrol.generated.h"

/**
 * 
 */
UCLASS()
class ANACHRONIA_API UBTService_Patrol : public UBTService
{
	GENERATED_BODY()
public:
	UBTService_Patrol();

protected:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
