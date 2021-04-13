#include "GuardPawnMovementComponent.h"

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
	GEngine->AddOnScreenDebugMessage(41, 5.0f, FColor::Yellow, Msg);;
	
	if (Controller->IsFollowingAPath())
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

void UGuardPawnMovementComponent::MoveToLocation(FVector DesiredLocation, float DeltaTime)
{
	AController* Controller = PawnOwner->GetController();
	if (Controller->IsFollowingAPath())
	{
		return;
	}

	const FVector OldLocation = UpdatedComponent->GetComponentLocation();
	const FVector DeltaMove = DesiredLocation - OldLocation;
	
	FHitResult Hit;
	const FQuat Rotation = UpdatedComponent->GetComponentQuat();
	SafeMoveUpdatedComponent(DeltaMove, Rotation, true, Hit);
	if (Hit.IsValidBlockingHit())
	{
		SlideAlongSurface(DeltaMove, 1.0f - Hit.Time, Hit.Normal, Hit);
	}

	const FVector NewLocation = UpdatedComponent->GetComponentLocation();
	Velocity = (NewLocation - OldLocation) / DeltaTime;

	UpdateComponentVelocity();
}
