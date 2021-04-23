// Fill out your copyright notice in the Description page of Project Settings.


#include "GuardPawn.h"
#include "GuardAIController.h"
#include "GuardPatrolPath.h"
#include "Components/SplineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "AIController.h"
#include "DrawDebugHelpers.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"

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
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bUseRVOAvoidance = true;
	GetCharacterMovement()->AvoidanceConsiderationRadius = 250.0f;
	GetCharacterMovement()->AvoidanceWeight = 0.5f;

	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight()));

	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("PerceptionComponent");
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>("SightConfig");
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>("HearingConfig");
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
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
		const FRotator NewRotation = Spline->GetRotationAtSplinePoint(Point, ESplineCoordinateSpace::World);
		SetActorLocation(NewLocation);
		if (GetController())
		{
			GetController()->SetControlRotation(NewRotation);
			GetGuardAI()->MakeThisOriginalRotation();
		}
		
		SetActorRotation(NewRotation);
	}

	PerceptionComponent->ConfigureSense(*SightConfig);
	PerceptionComponent->ConfigureSense(*HearingConfig);

	//SetDamageToCurrentHealth(1000.0f, false);
}

// Called every frame
void AGuardPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector EyesLoc;
	FRotator EyesRot;
	GetActorEyesViewPoint(EyesLoc, EyesRot);
	DrawDebugDirectionalArrow(GetWorld(), EyesLoc, EyesLoc + EyesRot.Vector() * 100.0f, 10.0f, FColor::White, false, -1, 0, 1.0f);
}

void AGuardPawn::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	if (PerceptionComponent != nullptr)
	{
		if (SightConfig)
		{
			UE_LOG(LogTemp, Display, TEXT("Update sense config!"));
			SightConfig->SightRadius = SightRadius;
			SightConfig->LoseSightRadius = LoseSightRadius;
			SightConfig->PeripheralVisionAngleDegrees = PeripheralVisionHalfAngle;
			PerceptionComponent->RequestStimuliListenerUpdate();
		}
	}
}

AGuardAIController* AGuardPawn::GetGuardAI() const
{
	return Cast<AGuardAIController>(GetController());
}

void AGuardPawn::SetDamageToCurrentHealth(float Damage, bool bNonLethal)
{
	if (GetGuardAI() && GetGuardAI()->IsVulnerableToStealthTakeDown())
	{
		CurrentHealth = 0.0f;
	}
	else
	{
		CurrentHealth -= Damage;
	}
	
	if (CurrentHealth <= 0.0f)
	{
		// Die
		CurrentHealth = 0.0f;
		OnDeath(bNonLethal);
	}
}

void AGuardPawn::OnDeath_Implementation(bool bNonLethalDeath)
{
	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetSimulatePhysics(true);
	const FVector HeadLocation = GetActorLocation() + FVector::UpVector * GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 0.5f;
	FVector Force = GetActorForwardVector() * 40000.0f;
	//Force.Z += 30000.0f;
	GetCapsuleComponent()->AddImpulseAtLocation(Force, HeadLocation);
	if (GetController() != nullptr)
	{
		GetController()->UnPossess();
	}
}
