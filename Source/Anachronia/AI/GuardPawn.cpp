// Fill out your copyright notice in the Description page of Project Settings.


#include "GuardPawn.h"
#include "GuardAIController.h"
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
	GetCharacterMovement()->bUseControllerDesiredRotation = true;

	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight()));
}

// Called when the game starts or when spawned
void AGuardPawn::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGuardPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AGuardPawn::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	//if (bStartOnPath && MovementComponent != nullptr && MovementComponent->PatrolPath)
	//{
	//	FVector NewLocation;
	//	FRotator NewRotation;
	//	MovementComponent->FindClosestLocationAndRotationOnPath(GetActorLocation(), NewLocation, NewRotation);
	//	SetActorLocation(NewLocation);
	//	SetActorRotation(NewRotation);
	//}
	//else
	//{
	//	bStartOnPath = false;
	//}
}
