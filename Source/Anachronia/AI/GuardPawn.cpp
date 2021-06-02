// Fill out your copyright notice in the Description page of Project Settings.


#include "GuardPawn.h"
#include "GuardAIController.h"
#include "GuardPatrolPath.h"
#include "Components/SplineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "AIController.h"
#include "DrawDebugHelpers.h"
#include "Perception/AISenseConfig_Sight.h"

#define ECC_Guard ECC_GameTraceChannel2

// Sets default values
AGuardPawn::AGuardPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AGuardAIController::StaticClass();
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));
	GetCapsuleComponent()->SetCollisionObjectType(ECC_Guard);
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bUseRVOAvoidance = true;
	GetCharacterMovement()->AvoidanceConsiderationRadius = 250.0f;
	GetCharacterMovement()->AvoidanceWeight = 0.5f;
	Tags.Add(TEXT("Guard"));

	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight()));

	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("PerceptionComponent");
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>("SightConfig");
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
}

// Called when the game starts or when spawned
void AGuardPawn::BeginPlay()
{
	Super::BeginPlay();
	
	Respawn();

	//SetDamageToCurrentHealth(1000.0f, false);
}

// Called every frame
void AGuardPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#if !UE_BUILD_SHIPPING
	if (bDebugInfo)
	{
		FVector EyesLoc;
		FRotator EyesRot;
		GetActorEyesViewPoint(EyesLoc, EyesRot);
		DrawDebugDirectionalArrow(GetWorld(), EyesLoc, EyesLoc + EyesRot.Vector() * 100.0f, 10.0f, FColor::White, false, -1, 0, 1.0f);
	}
#endif
}

AGuardAIController* AGuardPawn::GetGuardAI() const
{
	return Cast<AGuardAIController>(GetController());
}

void AGuardPawn::GetActorEyesViewPoint(FVector& Location, FRotator& Rotation) const
{
	// Default pawn implementation will use controller rotation instead of the visual rotation of the pawn
	Location = GetPawnViewLocation();
	Rotation = GetActorRotation();
}

void AGuardPawn::ConfigureSenses()
{
	if (SightConfig == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Told to configure senses, but senses are null. See: %p"), SightConfig);
		return;
	}
	SightConfig->SightRadius = SightRadius;
	SightConfig->LoseSightRadius = LoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = PeripheralVisionHalfAngle;
	PerceptionComponent->RequestStimuliListenerUpdate();
}

FRotator AGuardPawn::FindBestDeadRotation()
{
	const FRotator OrigRot = GetActorRotation();
	FRotator BestRotation = OrigRot;
	float BestTraceTime = 0;
	const float TraceLength = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const FVector Start = GetActorLocation();
	
	FCollisionQueryParams Params;
	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors);
	Params.AddIgnoredActors(AttachedActors);
	
	for (int i = 0; i < 4; i++)
	{
		FRotator Rotation = OrigRot;
		Rotation.Yaw += i * 45.0f;
		SetActorRotation(Rotation, ETeleportType::TeleportPhysics);
		FVector Offset = GetActorForwardVector() * TraceLength;

		float TraceTime = 0;
		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(Hit, Start, Start + Offset, ECC_Visibility, Params);
		TraceTime += Hit.Time;
		GetWorld()->LineTraceSingleByChannel(Hit, Start, Start - Offset, ECC_Visibility, Params);
		TraceTime += Hit.Time;

		UE_LOG(LogTemp, Display, TEXT("Angle: %f w/ time: %f, hit: %s"), Rotation.Yaw, TraceTime, Hit.Actor.IsValid() ? *Hit.Actor->GetName() : TEXT("nullptr"));
		
		if (TraceTime > BestTraceTime)
		{
			BestTraceTime = TraceTime;
			BestRotation = Rotation;
		}
	}

	return BestRotation;
}

TSubclassOf<AActor> AGuardPawn::FindVoiceLineAsset(TSubclassOf<AActor> BaseAsset, int VoiceIndex)
{
	const FString BaseName = BaseAsset->GetName().Replace(TEXT("_C"), TEXT(""));
	// VL_Guard_1_LarmarAndra
	const FString LineWithoutGuard = BaseName.RightChop(11);
	const FString NewName = FString::Printf(TEXT("VL_Guard_%d_%s"), VoiceIndex, *LineWithoutGuard);
	// Blueprint'/Game/Anachronia/Blueprints/VoiceSystem/VoiceLines_Guards/VL_guard_1/VL_Guard_1_Larmar_andra_1.VL_Guard_1_Larmar_andra_1'
	const FString NewAssetPath = FString::Printf(TEXT("/Game/Anachronia/Blueprints/VoiceSystem/VoiceLines_Guards/VL_guard_%d/%s.%s_C"), VoiceIndex, *NewName, *NewName);

	UE_LOG(LogTemp, Display, TEXT("Find voiceline: %s -> %s"), *BaseName, *NewAssetPath);
	
	const TSubclassOf<AActor> Asset = LoadObject<UClass>(nullptr, *NewAssetPath);
	if (Asset)
	{
		UE_LOG(LogTemp, Display, TEXT("Voiceline found!"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Voiceline not found. Requested %s, tried to load %s"), *BaseName, *NewAssetPath));
		UE_LOG(LogTemp, Display, TEXT("Voiceline not found"));
	}
	
	return Asset;
}

void AGuardPawn::Respawn()
{
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

	CurrentHealth = MaxHealth;

	GetCharacterMovement()->SetMovementMode(GetCharacterMovement()->DefaultLandMovementMode);
	GetCharacterMovement()->bUseRVOAvoidance = true;
	GetCapsuleComponent()->SetSimulatePhysics(false);
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	OnRespawn();
}

bool AGuardPawn::CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation,
	int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor) const
{
	EGuardState State =  GetGuardAI()->GetState();
	if ((State != EGuardState::Dead && State != EGuardState::Hunt) || GetActorEnableCollision() == false)
	{
		// Guards only have reason to be able to see dead guards and guards who are hunting the player, and collision-less guards (e.g. carried) should be invisible
		return false;
	}

	const float HalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const float ZOffsetList[]{ HalfHeight, 0.0f, -HalfHeight };

	const float Range = FMath::Lerp(BodyCanBeSeenFromRange, BodyCanBeSeenFromRange * Luminance, BodyCanSeeLuminanceInfluence);

	FCollisionQueryParams Params(SCENE_QUERY_STAT(AILineOfSight), true, IgnoreActor);
	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors);
	Params.AddIgnoredActors(AttachedActors);
	
	for (float ZOffset : ZOffsetList)
	{
		const FVector TargetLocation = GetActorLocation() + GetActorUpVector() * ZOffset;
		
		if (FVector::Distance(ObserverLocation, TargetLocation) <= Range)
		{
			FHitResult HitResult;
			const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, ObserverLocation, TargetLocation,
				ECC_Visibility, Params);

			NumberOfLoSChecksPerformed++;

			if (bHit)
			{
				UE_LOG(LogTemp, Display, TEXT("Blocked by %s"), HitResult.Actor.IsValid() ? *HitResult.Actor->GetName() : TEXT("nullptr"));
			}
			
			if (!bHit)
			{
				// If this trace was blacked by nothing, or blocked by target itself (somehow), we can be seen
				OutSeenLocation = TargetLocation;
				OutSightStrength = 1.0f;
				return true;
			}
		}
	}

	OutSightStrength = 0.0f;
	return false;
}

void AGuardPawn::SetDamageToCurrentHealth(float Damage, bool bNonLethal)
{
	const float OldHealth = CurrentHealth;
	
	CurrentHealth -= Damage;
	
	if (CurrentHealth <= 0.0f)
	{
		// Die
		CurrentHealth = 0.0f;
		GetCapsuleComponent()->SetGenerateOverlapEvents(false);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		GetCharacterMovement()->DisableMovement();
		GetCharacterMovement()->bUseRVOAvoidance = false;
		OnDeath(bNonLethal);
		GetGuardAI()->Die();
	}
	else
	{
		OnDamaged(CurrentHealth, OldHealth);
	}
}

void AGuardPawn::OnDeath_Implementation(bool bNonLethalDeath)
{
	GetCapsuleComponent()->SetSimulatePhysics(true);
	const FVector HeadLocation = GetActorLocation() + FVector::UpVector * GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 0.5f;
	FVector Force = GetActorForwardVector() * 40000.0f;
	//Force.Z += 30000.0f;
	GetCapsuleComponent()->AddImpulseAtLocation(Force, HeadLocation);
}
