#pragma once

#include "CoreMinimal.h"

#include "GameFramework/PawnMovementComponent.h"
#include "GuardPawnMovementComponent.generated.h"

class AGuardPatrolPath;
class GuardAIController;

UENUM(BlueprintType)
enum class EGuardPathFollowState : uint8
{
	Stopped,
	Following,
	TooFarAway,
	Blocked,
};

/**
 * 
 */
UCLASS()
class ANACHRONIA_API UGuardPawnMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

public:
	UGuardPawnMovementComponent();

	UPROPERTY(Category = Guard, EditInstanceOnly)
	AGuardPatrolPath* PatrolPath;
	
	UPROPERTY(Category = Guard, EditAnywhere, BlueprintReadWrite)
	float WalkSpeed = 200.0f;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual float GetMaxSpeed() const override;
	void StartFollowingPath();
	void StopFollowingPath(EGuardPathFollowState NewState = EGuardPathFollowState::Stopped);
	void FindClosestLocationAndRotationOnPath(FVector FromLocation, FVector& OutLocation, FRotator& OutRotation) const;

	FORCEINLINE EGuardPathFollowState GetPathFollowState() const { return PathFollowState; }
	FORCEINLINE FVector GetPathReturnToLocation() const { return PathReturnToLocation; }

private:
	FRotator DesiredRotation;
	float DistanceAlongPath;
	float PatrolStopTimer;
	float PatrolDirection = 1.0f;
	FVector PathReturnToLocation;

	UPROPERTY(VisibleAnywhere, Transient)
	EGuardPathFollowState PathFollowState;

	void StepAlongPatrolPath(float DeltaTime);
	bool WouldPenetrateAt(FVector Location, FQuat Rotation);

	/**
	 * Returns true if close enough to be considered being on the path. If not close, will update PathReturnToLocation.
	 */
	bool CheckIfCloseEnoughToPatrolPath();
};
