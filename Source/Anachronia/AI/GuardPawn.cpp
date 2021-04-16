// Fill out your copyright notice in the Description page of Project Settings.


#include "GuardPawn.h"
#include "GuardAIController.h"
#include "GuardPatrolPath.h"
#include "Components/SplineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AGuardPawn::AGuardPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AGuardAIController::StaticClass();
	GetCapsuleComponent()->SetCollisionProfileName("Pawn");
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseRVOAvoidance = true;
	GetCharacterMovement()->AvoidanceConsiderationRadius = 250.0f;
	GetCharacterMovement()->AvoidanceWeight = 0.5f;

	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight()));
}

// Called when the game starts or when spawned
void AGuardPawn::BeginPlay()
{
	Super::BeginPlay();

	if (bStartOnPath && PatrolPath != nullptr)
	{
		const int32 Point = PatrolPath->FindClosestPointToWorldLocation(GetActorLocation());
		USplineComponent* Spline = PatrolPath->GetSpline();
		const FVector NewLocation = Spline->GetLocationAtSplinePoint(Point, ESplineCoordinateSpace::World);
		const FQuat NewRotation = Spline->GetQuaternionAtSplinePoint(Point, ESplineCoordinateSpace::World);
		SetActorLocation(NewLocation);
		SetActorRotation(NewRotation);
	}
}

// Called every frame
void AGuardPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}