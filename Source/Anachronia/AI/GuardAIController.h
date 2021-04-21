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

UENUM(BlueprintType)
enum class EMovementSpeed : uint8
{
	Walk,
	Run,
};

UENUM(BlueprintType)
enum class EAlertness : uint8
{
	Neutral,
	Distracted,
	AlarmedKnowing,
	AlarmedUnknowing,
};

UENUM(BlueprintType)
enum class EGuardState : uint8
{
	Patrol,
	Distracted,
	Inspect,
	Hunt,
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
	float GetAlertnessValue(EAlertness AlertnessLevel) const;
	void PickNextPatrolPoint();
	void FindClosestPatrolPoint();
	FPatrolStop* GetCurrentPatrolStopInfo() const;
	bool IsSusEnough(ESusLevel Level) const;
	void MakeThisOriginalRotation();
	void ResetRotation();
	void SetMovementSpeed(EMovementSpeed NewSpeed);
	void SetAlertness(EAlertness InAlertness);
	void SetState(EGuardState InState);

	UFUNCTION(BlueprintCallable)
	ESusLevel GetSusLevel() const;

protected:
	UPROPERTY(Transient, BlueprintReadOnly, VisibleInstanceOnly)
	float SusValue;

	UPROPERTY(Transient, BlueprintReadOnly, VisibleInstanceOnly)
	bool bCanSeePlayer;

	UPROPERTY(Transient, BlueprintReadOnly, VisibleInstanceOnly)
	EAlertness Alertness;

	UPROPERTY(Transient, BlueprintReadOnly, VisibleInstanceOnly)
	EGuardState State;
	
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

	FRotator OriginalRotation;
	FVector OriginalLocation;

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

private:
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
	
};
