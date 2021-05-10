#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "GuardAIController.generated.h"

struct FAnachroniaNoiseInfo;
class AAnachroniaPlayer;
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
	Dead,
};

UENUM(BlueprintType)
enum class EGuardState : uint8
{
	Dead,
	Patrol,
	Distracted,
	Inspect,
	InspectLookAround,
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
	void SetPatrolPath(AGuardPatrolPath* Path);
	AGuardPatrolPath* GetPatrolPath() const;
	FVector GetCurrentPatrolGoal() const;
	float GetAlertnessValue(EAlertness AlertnessLevel) const;
	void PickNextPatrolPoint();
	void FindClosestPatrolPoint();
	FPatrolStop* GetCurrentPatrolStopInfo() const;
	void ChooseNewPatrolPath();
	bool IsSusEnough(ESusLevel Level) const;
	void MakeThisOriginalRotation();
	void ResetRotation();
	void SetMovementSpeed(EMovementSpeed NewSpeed);
	void SetAlertness(EAlertness InAlertness);
	FORCEINLINE EAlertness GetAlertness() const { return Alertness; }
	void SetState(EGuardState InState);
	FORCEINLINE EGuardState GetState() const { return State; }
	virtual void FindPathForMoveRequest(const FAIMoveRequest& MoveRequest, FPathFindingQuery& Query, FNavPathSharedPtr& OutPath) const override;
	UFUNCTION(BlueprintCallable)
	ESusLevel GetSusLevel() const;
	FORCEINLINE bool CanSeePlayer() const { return bCanSeePlayer; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE AAnachroniaPlayer* GetPlayer() const { return PlayerRef; }

	UFUNCTION(BlueprintCallable)
	void Respawn();

	UFUNCTION(BlueprintCallable)
	bool IsVulnerableToStealthTakeDown() const;

	UFUNCTION(BlueprintCallable)
	void Die();
	
	bool CanAttackPlayer();

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
	AAnachroniaPlayer* PlayerRef;

	UPROPERTY(Transient, VisibleInstanceOnly)
	int32 NextPatrolPoint = 0;
	
	int32 PatrolDirection = 1;

	FRotator OriginalRotation;
	FVector OriginalLocation;

	UPROPERTY()
	AGuardPatrolPath* OriginalPatrolPath;
	
	float AttackCooldownTimer;
	float BackupTimer;
	
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	UPROPERTY()
	TArray<AActor*> AllGuards;
	
	static EGuardState CurrentHighestState;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static EGuardState GetGlobalGuardAlertnessState();
	
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	void OnAnachroniaNoise(FAnachroniaNoiseInfo NoiseInfo);

	int32 LineTraceSound(FVector Start, FVector End) const;
	
	FORCEINLINE bool ShouldShowDebug() const;
};
