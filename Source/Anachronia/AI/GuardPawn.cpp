// Fill out your copyright notice in the Description page of Project Settings.


#include "GuardPawn.h"
#include "GuardPawnMovementComponent.h"
#include "GuardAIController.h"

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
	SetActorEnableCollision(true);
	CapsuleComponent->SetCollisionProfileName(TEXT("Pawn"));
	CapsuleComponent->SetCanEverAffectNavigation(false);

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -CapsuleComponent->GetScaledCapsuleHalfHeight()));
	MeshComponent->SetCanEverAffectNavigation(false);
	
	MovementComponent = CreateDefaultSubobject<UGuardPawnMovementComponent>(TEXT("MovementComponent"));
	MovementComponent->UpdatedComponent = RootComponent;
}

// Called when the game starts or when spawned
void AGuardPawn::BeginPlay()
{
	Super::BeginPlay();

	bStartOnPath = false; // Don't want to keep snapping to path if moved in editor while playing for debug purposes
}

// Called every frame
void AGuardPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!GetVelocity().IsNearlyZero())
	{
		const FVector MovementDirection = GetVelocity().GetUnsafeNormal2D();
		const FRotator DesiredRotation = MovementDirection.ToOrientationRotator();
		SetActorRotation(FMath::RInterpConstantTo(GetActorRotation(), DesiredRotation, DeltaTime, TurnSpeed));
	}
}


void AGuardPawn::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (bStartOnPath && MovementComponent != nullptr && MovementComponent->PatrolPath)
	{
		FVector NewLocation;
		FRotator NewRotation;
		MovementComponent->FindClosestLocationAndRotationOnPath(GetActorLocation(), NewLocation, NewRotation);
		SetActorLocation(NewLocation);
		SetActorRotation(NewRotation);
	}
	else
	{
		bStartOnPath = false;
	}
}
