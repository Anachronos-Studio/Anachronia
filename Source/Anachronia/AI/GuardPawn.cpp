// Fill out your copyright notice in the Description page of Project Settings.


#include "GuardPawn.h"
#include "GuardPawnMovementComponent.h"
#include "GuardAIController.h"
#include "GuardPatrolPath.h"
#include "Components/SplineComponent.h"

// Sets default values
AGuardPawn::AGuardPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AGuardAIController::StaticClass();

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	RootComponent = CapsuleComponent;
	CapsuleComponent->SetCapsuleHalfHeight(88.0f);
	CapsuleComponent->SetCapsuleRadius(22.0f);

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -CapsuleComponent->GetScaledCapsuleHalfHeight()));
	
	MovementComponent = CreateDefaultSubobject<UGuardPawnMovementComponent>(TEXT("MovementComponent"));
	MovementComponent->UpdatedComponent = RootComponent;
}

// Called when the game starts or when spawned
void AGuardPawn::BeginPlay()
{
	Super::BeginPlay();

	bStartOnPath = false; // Don't want to keep snapping to path if moved in editor while playing for debug purposes

	UE_LOG(LogTemp, Display, TEXT("ID: %d"), GetUniqueID());
}

// Called every frame
void AGuardPawn::StartFollowingPath()
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
void AGuardPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (PathFollowState == EGuardPathFollowState::Following)
	{
		StepAlongPatrolPath(DeltaTime);
	}

	SetActorRotation(FMath::RInterpConstantTo(GetActorRotation(), DesiredRotation, DeltaTime, TurnSpeed));
}

void AGuardPawn::StopFollowingPath(EGuardPathFollowState NewState)
{
	PathFollowState = NewState;
	PatrolStopTimer = -1.0f;
}

void AGuardPawn::StepAlongPatrolPath(float DeltaTime)
{
	if (PatrolPath == nullptr)
	{
		StopFollowingPath();
	}

	if (!CheckIfCloseEnoughToPatrolPath())
	{
		StopFollowingPath(EGuardPathFollowState::TooFarAway);
	}
	
	if (PatrolStopTimer > 0.0f)
	{
		PatrolStopTimer -= DeltaTime;
	}
	else
	{
		USplineComponent* Spline = PatrolPath->GetSpline();
		const bool bLoop = Spline->IsClosedLoop();

		const float OldDistance = DistanceAlongPath;
		float NewDistance = DistanceAlongPath + MovementComponent->WalkSpeed * DeltaTime * PatrolDirection;
		
		if (PatrolPath->PatrolStops.Num() > 0)
		{
			int32 NumKeys = Spline->GetNumberOfSplinePoints();
			if (!bLoop)
			{
				NumKeys = NumKeys - 1;
			}
			// In the spline editor, it looks like Input Keys go from 0.0 to <Number of points> along the whole spline
			// But the return value of GetInputKey... is apparently always in the range 0.0-1.0 instead
			const float OldInputKey = Spline->GetInputKeyAtDistanceAlongSpline(OldDistance) * NumKeys;
			const float NewInputKey = Spline->GetInputKeyAtDistanceAlongSpline(NewDistance) * NumKeys;

			//GEngine->AddOnScreenDebugMessage(41, 2.0, FColor::Yellow, FString::Printf(TEXT("%f"), NewInputKey));
			
			for (FPatrolStop& StopPoint : PatrolPath->PatrolStops)
			{
				bool bPassedStop;
				if (PatrolDirection > 0.0f) bPassedStop = NewInputKey >= StopPoint.InputKey && OldInputKey < StopPoint.InputKey;
				else bPassedStop = NewInputKey <= StopPoint.InputKey && OldInputKey > StopPoint.InputKey;
				if (bPassedStop)
				{
					NewDistance = PatrolPath->GetDistanceAlongSplineAtSplineInputKey(StopPoint.InputKey);
					PatrolStopTimer = StopPoint.Duration;
					break;
				}
			}
		}

		const float PathLength = Spline->GetSplineLength();
		if (bLoop)
		{
			NewDistance = FMath::Fmod(NewDistance, PathLength);
		}
		else if (PatrolPath->bPingPong)
		{
			if (NewDistance > PathLength && PatrolDirection > 0.0f)
			{
				NewDistance = PathLength - (NewDistance - PathLength);
				PatrolDirection  = -PatrolDirection;
			}
			else if (NewDistance < 0.0f && PatrolDirection < 0.0f)
			{
				NewDistance = -NewDistance;
				PatrolDirection = -PatrolDirection;
			}
		}

		DistanceAlongPath = NewDistance;
		
		FVector NewLocation = Spline->GetLocationAtDistanceAlongSpline(NewDistance, ESplineCoordinateSpace::World);
		NewLocation.Z = GetActorLocation().Z;
		MovementComponent->MoveToLocation(NewLocation, DeltaTime);
		const FVector NewDirection = Spline->GetDirectionAtDistanceAlongSpline(NewDistance, ESplineCoordinateSpace::World) * PatrolDirection;
		const FRotator NewRotation = NewDirection.Rotation();
		DesiredRotation = NewRotation;
	}
}

bool AGuardPawn::CheckIfCloseEnoughToPatrolPath()
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

void AGuardPawn::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (bStartOnPath && PatrolPath != nullptr && PatrolPath->GetSpline() != nullptr)
	{
		FVector NewLocation = PatrolPath->GetSpline()->FindLocationClosestToWorldLocation(GetActorLocation(), ESplineCoordinateSpace::World);
		NewLocation.Z = GetActorLocation().Z;
		SetActorLocation(NewLocation);
		const FRotator NewRotation = PatrolPath->GetSpline()->FindRotationClosestToWorldLocation(GetActorLocation(), ESplineCoordinateSpace::World);
		SetActorRotation(NewRotation);
	}
}
