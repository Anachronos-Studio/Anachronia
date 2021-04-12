

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GuardAIController.generated.h"

class AGuardPatrolPath;
class AGuardPawn;

/**
 * 
 */
UCLASS()
class ANACHRONIA_API AGuardAIController : public AAIController
{
	GENERATED_BODY()

public:
	AGuardAIController();
	virtual void BeginPlay() override;
	AGuardPawn* GetGuardPawn() const;

protected:
	UPROPERTY(VisibleInstanceOnly)
	AGuardPawn* GuardPawn;

	UPROPERTY()
	UBehaviorTree* BTAsset;

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	
};
