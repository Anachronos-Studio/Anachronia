#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "GuardAIController.generated.h"

struct FPatrolStop;
class AGuardPatrolPath;
class AGuardPawn;

UENUM(BlueprintType)
enum class ESusLevel : uint8
{
	NotSus,
	NonZero,
	KindaSus,
	Inspect,
	Busted,
};

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
	virtual void Tick(float DeltaTime) override;
	AGuardPawn* GetGuardPawn() const;
	AGuardPatrolPath* GetPatrolPath() const;
	FVector GetCurrentPatrolGoal() const;
	void PickNextPatrolPoint();
	void FindClosestPatrolPoint();
	FPatrolStop* GetCurrentPatrolStopInfo() const;
	bool IsSusEnough(ESusLevel Level) const;

	UFUNCTION(BlueprintCallable)
	ESusLevel GetSusLevel() const;

protected:
	UPROPERTY(Transient, BlueprintReadOnly, VisibleInstanceOnly)
	float SusValue;

	UPROPERTY(Transient, BlueprintReadOnly, VisibleInstanceOnly)
	bool bCanSeePlayer;
	
private:
	UPROPERTY(VisibleInstanceOnly)
	AGuardPawn* GuardPawn;

	UPROPERTY()
	UBehaviorTree* BTAsset;

	UPROPERTY()
	AActor* PlayerRef;

	UPROPERTY(Transient, VisibleInstanceOnly)
	int32 NextPatrolPoint = 0;
	
	int32 PatrolDirection = 1;

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

private:
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
	
};
