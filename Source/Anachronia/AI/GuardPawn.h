// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Pawn.h"
#include "GuardPawn.generated.h"

class AGuardPatrolPath;
class UGuardPawnMovementComponent;

UENUM(BlueprintType)
enum class EGuardPathFollowState : uint8
{
	Stopped,
	Following,
	TooFarAway,
	Blocked,
};

UCLASS()
class ANACHRONIA_API AGuardPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGuardPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void StartFollowingPath();
	void StopFollowingPath(EGuardPathFollowState NewState = EGuardPathFollowState::Stopped);

	FORCEINLINE EGuardPathFollowState GetPathFollowState() const { return PathFollowState; }
	FORCEINLINE FVector GetPathReturnToLocation() const { return PathReturnToLocation; }

protected:
	UPROPERTY(Category = Guard, EditInstanceOnly)
	AGuardPatrolPath* PatrolPath;

	// If true, will snap the pawn to the path spline before playing
	UPROPERTY(Category = Guard, EditInstanceOnly, Meta = (EditCondition = "PatrolPath != nullptr"))
	bool bStartOnPath;

	UPROPERTY(Category = Guard, EditAnywhere)
	float TurnSpeed = 800.0f;

	UPROPERTY(VisibleAnywhere)
	UGuardPawnMovementComponent* MovementComponent;

	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* CapsuleComponent;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* MeshComponent;
	
private:
	FRotator DesiredRotation;
	float DistanceAlongPath;
	float PatrolStopTimer;
	float PatrolDirection = 1.0f;
	FVector PathReturnToLocation;
	EGuardPathFollowState PathFollowState;

	void StepAlongPatrolPath(float DeltaTime);

	/**
	 * Returns true if close enough to be considered being on the path. If not close, will update PathReturnToLocation.
	 */
	bool CheckIfCloseEnoughToPatrolPath();
	
	virtual void OnConstruction(const FTransform& Transform) override;
};