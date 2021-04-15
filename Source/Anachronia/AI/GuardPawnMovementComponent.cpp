#include "GuardPawnMovementComponent.h"

#include "GuardPatrolPath.h"
#include "Components/SplineComponent.h"

/*
 * No longer used
 */

UGuardPawnMovementComponent::UGuardPawnMovementComponent()
{
	NavAgentProps.bCanWalk = true;
}

void UGuardPawnMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PawnOwner || !UpdatedComponent)
	{
		return;
	}

	AController* Controller = PawnOwner->GetController();

	FVector Input = GetPendingInputVector().GetClampedToMaxSize(1.0f);
	FString Msg = FString::Printf(TEXT("Input: %s\nVel: %s\nbPathing: %d"), *Input.ToCompactString(), *Velocity.ToString(), PawnOwner->GetController()->IsFollowingAPath());
	//GEngine->AddOnScreenDebugMessage(41, 5.0f, FColor::Yellow, Msg);;

	if (PathFollowState == EGuardPathFollowState::Following)
	{
		StepAlongPatrolPath(DeltaTime);	
	}
	else if (Controller->IsFollowingAPath())
	{
		Velocity = Velocity.GetClampedToMaxSize2D(GetMaxSpeed());
		
		const FVector DeltaMove = Velocity * DeltaTime;

		if (!DeltaMove.IsNearlyZero())
		{
			FHitResult Hit;
			const FQuat Rotation = UpdatedComponent->GetComponentQuat();
			SafeMoveUpdatedComponent(DeltaMove, Rotation, true, Hit);
			if (Hit.IsValidBlockingHit())
			{
				SlideAlongSurface(DeltaMove, 1.0f - Hit.Time, Hit.Normal, Hit);
			}
		}
	}

	UpdateComponentVelocity();
}

float UGuardPawnMovementComponent::GetMaxSpeed() const
{
	return WalkSpeed;
}

void UGuardPawnMovementComponent::StartFollowingPath()
{
	if (PatrolPath == nullptr) return;

	USplineComponent* Spline = PatrolPath->GetSpline();
	const FVector CurrentLocation = GetActorLocation();
	const float ClosestInputKey = Spline->FindInputKeyClosestToWorldLocation(CurrentLocation);

	PatrolStopTimer = -1.0f;

	if (CheckIfCloseEnoughToPatrolPath())
	{
		DistanceAlongPath = PatrolPath->GetDistanceAlongSplineAtSplineInputKey(ClosestInputKey);
		PathFollowState = EGuardPathFollowState::Following;
	}
	else
	{
		StopFollowingPath(EGuardPathFollowState::TooFarAway);
	}
}

void UGuardPawnMovementComponent::StopFollowingPath(EGuardPathFollowState NewState)
{
	PathFollowState = NewState;
	PatrolStopTimer = -1.0f;
}

void UGuardPawnMovementComponent::FindClosestLocationAndRotationOnPath(FVector FromLocation, FVector& OutLocation, FRotator& OutRotation) const
{
	if (PatrolPath == nullptr || PatrolPath->GetSpline() == nullptr)
	{
		return;
	}
	
	OutLocation = PatrolPath->GetSpline()->FindLocationClosestToWorldLocation(GetActorLocation(), ESplineCoordinateSpace::World);
	OutLocation.Z = FromLocation.Z;
	OutRotation = PatrolPath->GetSpline()->FindRotationClosestToWorldLocation(GetActorLocation(), ESplineCoordinateSpace::World);
}

void UGuardPawnMovementComponent::StepAlongPatrolPath(float DeltaTime)
{
	//if (PatrolPath == nullptr)
	//{
	//	StopFollowingPath();
	//}

	//if (!CheckIfCloseEnoughToPatrolPath())
	//{
	//	StopFollowingPath(EGuardPathFollowState::TooFarAway);
	//}

	//if (PatrolStopTimer > 0.0f)
	//{
	//	PatrolStopTimer -= DeltaTime;
	//}
	//else
	//{
	//	USplineComponent* Spline = PatrolPath->GetSpline();
	//	const bool bLoop = Spline->IsClosedLoop();

	//	const float OldDistance = DistanceAlongPath;
	//	float NewDistance = DistanceAlongPath + WalkSpeed * DeltaTime * PatrolDirection;

	//	if (PatrolPath->PatrolStops.Num() > 0)
	//	{
	//		int32 NumKeys = Spline->GetNumberOfSplinePoints();
	//		if (!bLoop)
	//		{
	//			NumKeys = NumKeys - 1;
	//		}
	//		// In the spline editor, it looks like Input Keys go from 0.0 to <Number of points> along the whole spline
	//		// But the return value of GetInputKey... is apparently always in the range 0.0-1.0 instead
	//		const float OldInputKey = Spline->GetInputKeyAtDistanceAlongSpline(OldDistance) * NumKeys;
	//		const float NewInputKey = Spline->GetInputKeyAtDistanceAlongSpline(NewDistance) * NumKeys;

	//		//GEngine->AddOnScreenDebugMessage(41, 2.0, FColor::Yellow, FString::Printf(TEXT("%f"), NewInputKey));

	//		for (FPatrolStop& StopPoint : PatrolPath->PatrolStops)
	//		{
	//			bool bPassedStop;
	//			if (PatrolDirection > 0.0f) bPassedStop = NewInputKey >= StopPoint.InputKey && OldInputKey < StopPoint.InputKey;
	//			else bPassedStop = NewInputKey <= StopPoint.InputKey && OldInputKey > StopPoint.InputKey;
	//			if (bPassedStop)
	//			{
	//				NewDistance = PatrolPath->GetDistanceAlongSplineAtSplineInputKey(StopPoint.InputKey);
	//				PatrolStopTimer = StopPoint.Duration;
	//				break;
	//			}
	//		}
	//	}

	//	const float PathLength = Spline->GetSplineLength();
	//	if (bLoop)
	//	{
	//		NewDistance = FMath::Fmod(NewDistance, PathLength);
	//	}
	//	else if (PatrolPath->bPingPong)
	//	{
	//		if (NewDistance > PathLength && PatrolDirection > 0.0f)
	//		{
	//			NewDistance = PathLength - (NewDistance - PathLength);
	//			PatrolDirection = -PatrolDirection;
	//		}
	//		else if (NewDistance < 0.0f && PatrolDirection < 0.0f)
	//		{
	//			NewDistance = -NewDistance;
	//			PatrolDirection = -PatrolDirection;
	//		}
	//	}

	//	DistanceAlongPath = NewDistance;
	//	
	//	float LookAheadDistance = DistanceAlongPath + 100.0f;
	//	FVector LookAheadLocation = Spline->GetLocationAtDistanceAlongSpline(LookAheadDistance, ESplineCoordinateSpace::World);
	//	FQuat LookAheadRotation = Spline->GetQuaternionAtDistanceAlongSpline(LookAheadDistance, ESplineCoordinateSpace::World);

	//	WouldPenetrateAt(LookAheadLocation, LookAheadRotation);

	//	const FVector OldLocation = UpdatedComponent->GetComponentLocation();
	//	FVector DesiredLocation = Spline->GetLocationAtDistanceAlongSpline(NewDistance, ESplineCoordinateSpace::World);
	//	DesiredLocation.Z = OldLocation.Z;

	//	const FVector DeltaMove = DesiredLocation - OldLocation;

	//	FHitResult Hit;
	//	const FQuat Rotation = UpdatedComponent->GetComponentQuat();
	//	SafeMoveUpdatedComponent(DeltaMove, Rotation, true, Hit);
	//	//if (Hit.IsValidBlockingHit())
	//	//{
	//	//	SlideAlongSurface(DeltaMove, 1.0f - Hit.Time, Hit.Normal, Hit);
	//	//}

	//	const FVector NewLocation = UpdatedComponent->GetComponentLocation();
	//	Velocity = (NewLocation - OldLocation) / DeltaTime;
//	}
}

bool UGuardPawnMovementComponent::WouldPenetrateAt(FVector Location, FQuat Rotation)
{
	FHitResult Hit;
	const FVector OldLocation = UpdatedComponent->GetComponentLocation();
	const FQuat OldRotation = UpdatedComponent->GetComponentQuat();
	FVector Delta = Location - OldLocation;
	Delta.Z = 0;
	MoveUpdatedComponent(Delta, Rotation, false, &Hit, ETeleportType::ResetPhysics);
	MoveUpdatedComponent(-Delta, OldRotation, false, nullptr, ETeleportType::ResetPhysics);
	if (Hit.IsValidBlockingHit())
	{
		UE_LOG(LogTemp, Display, TEXT("About to penetrate!"));
		return true;
	}

	return false;
}

bool UGuardPawnMovementComponent::CheckIfCloseEnoughToPatrolPath()
{	
	const FVector ClosestLocationOnPath = PatrolPath->GetSpline()->FindLocationClosestToWorldLocation(GetActorLocation(), ESplineCoordinateSpace::World);
	FVector LocationDifference = ClosestLocationOnPath - GetActorLocation();
	LocationDifference.Z = 0;
	const bool bOnPath = LocationDifference.IsNearlyZero(1.0f);
	if (!bOnPath)
	{
		PathReturnToLocation = ClosestLocationOnPath;
		return false;
	}
	return true;
}
