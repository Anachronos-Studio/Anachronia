

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GuardAIController.generated.h"

struct FPatrolStop;
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
	AGuardPatrolPath* GetPatrolPath() const;
	FVector GetCurrentPatrolGoal() const;
	void PickNextPatrolPoint();
	void FindClosestPatrolPoint();
	FPatrolStop* GetCurrentPatrolStopInfo() const;

private:
	UPROPERTY(VisibleInstanceOnly)
	AGuardPawn* GuardPawn;

	UPROPERTY()
	UBehaviorTree* BTAsset;

	UPROPERTY(Transient, VisibleInstanceOnly)
	int32 NextPatrolPoint = 0;
	
	int32 PatrolDirection = 1;

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	
};
