#include "GuardAIController.h"

#include "DrawDebugHelpers.h"
#include "GuardPatrolPath.h"
#include "GuardPawn.h"
#include "NavigationSystem.h"
#include "Anachronia/PlayerCharacter/AnachroniaPlayer.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavMesh/NavMeshPath.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"

EGuardState AGuardAIController::CurrentHighestState = EGuardState::Patrol;

AGuardAIController::AGuardAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false; // Enable tick on possess instead (since tick requires a possessed pawn)
	bSetControlRotationFromPawnOrientation = false;
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BehaviorTreeAsset(TEXT("/Game/Anachronia/BehaviorTrees/BT_Guard"));
	checkf(BehaviorTreeAsset.Succeeded(), TEXT("Failed to load guard behavior tree asset"));
	BTAsset = BehaviorTreeAsset.Object;
}

void AGuardAIController::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Display, TEXT("AI play"));
	
	PlayerRef = Cast<AAnachroniaPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (PlayerRef == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Could not find a AnachroniaPlayer actor in the world!"));
	}

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGuardAIController::StaticClass(), AllGuards);
	CurrentHighestState = EGuardState::Patrol;
	
	GetBlackboardComponent()->SetValueAsObject("Player", PlayerRef);
}

void AGuardAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float Rate = GuardPawn->SusBaseIncreaseRate;
	if (bCanSeePlayer)
	{
		Rate *= GetAlertnessValue(Alertness);
		
		const float PlayerVisibility = PlayerRef->GetVisibility();
		const float Distance = FVector::Dist(GuardPawn->GetActorLocation(), PlayerRef->GetActorLocation());
		const float DistanceFactor = 1.0f - FMath::Clamp(Distance / GuardPawn->SightRadius, 0.0f, 1.0f);
		const bool bVeryClose = Distance <= GuardPawn->AlwaysSeeDistance;
		
		if (PlayerVisibility < GuardPawn->PlayerMinVisibility && !bVeryClose)
		{
			Rate = 0;
		}
		else
		{
			Rate *= PlayerVisibility;
		}
		
		Rate *= DistanceFactor * GuardPawn->SusDistanceRateMultiplier;

		if (bVeryClose)
		{
			Rate *= GuardPawn->VeryCloseSusIncreaseMultiplier;
		}
		
		if (ShouldShowDebug())
		{
			const FString Msg = FString::Printf(TEXT("Rate: %.3f\nDist: %.3f"),
				Rate,
				DistanceFactor
			);
			GEngine->AddOnScreenDebugMessage(420, 1.0f, FColor::White, Msg);
		}

		const ESusLevel OldSusLevel = GetSusLevel();
		SusValue = FMath::Min(SusValue + Rate * DeltaTime, 1.0f);
		const ESusLevel NewSusLevel = GetSusLevel();
		if (OldSusLevel != NewSusLevel)
		{
			GuardPawn->OnSusLevelIncreased(NewSusLevel, OldSusLevel);
		}
		
		if (IsSusEnough(ESusLevel::KindaSus) && PlayerRef)
		{
			if (State != EGuardState::Inspect) // When inspecting, should not track moving player, so don't update goal location
			{
				GetBlackboardComponent()->SetValueAsVector(TEXT("NavigationGoalLocation"), PlayerRef->GetActorLocation());
			}
		}
	}

	if (!bCanSeePlayer || Rate <= 0.0f)
	{
		float MinSus = 0.0f;
		if (Alertness == EAlertness::AlarmedKnowing)
		{
			MinSus = 1.0f;
		}
		else if (State == EGuardState::Inspect || State == EGuardState::InspectLookAround)
		{
			MinSus = GuardPawn->SusInspectThreshold;
		}

		const ESusLevel OldSusLevel = GetSusLevel();
		SusValue = FMath::Max(SusValue - GuardPawn->SusDecreaseRate * DeltaTime, MinSus);
		const ESusLevel NewSusLevel = GetSusLevel();
		if (OldSusLevel != NewSusLevel)
		{
			GuardPawn->OnSusLevelIncreased(NewSusLevel, OldSusLevel);
		}
	}

	if (ShouldShowDebug())
	{
		const FString Msg = FString::Printf(TEXT("Sus: %3.0f%% [%s]\nAlertness: %s\nMain state: %s\n%s\nHighest state: %s"),
			SusValue * 100.0f,
			*StaticEnum<ESusLevel>()->GetValueAsString(GetSusLevel()),
			*StaticEnum<EAlertness>()->GetValueAsString(Alertness),
			*StaticEnum<EGuardState>()->GetValueAsString(State),
			bCanSeePlayer ? TEXT("Player in sight") : TEXT("Can't see player"),
			*StaticEnum<EGuardState>()->GetValueAsString(CurrentHighestState)
		);
		GEngine->AddOnScreenDebugMessage(419, 1.0f, FColor::White, Msg);

		const FVector NavigationGoalLocation = GetBlackboardComponent()->GetValueAsVector("NavigationGoalLocation");
		const FVector PredictedPlayerLocation = GetBlackboardComponent()->GetValueAsVector("PredictedPlayerLocation");
		const FVector NoiseLocation = GetBlackboardComponent()->GetValueAsVector("NoiseLocation");
		DrawDebugCrosshairs(GetWorld(), NavigationGoalLocation, FRotator(0.0f, 0.0f, 0.0f), 50.0f, FColor::Green);
		DrawDebugDirectionalArrow(GetWorld(), GuardPawn->GetActorLocation(), NavigationGoalLocation, 50.0f, FColor::Green);
		DrawDebugCrosshairs(GetWorld(), PredictedPlayerLocation, FRotator(0.0f, 20.0f, 0.0f), 50.0f, FColor::Cyan);
		DrawDebugDirectionalArrow(GetWorld(), NavigationGoalLocation, PredictedPlayerLocation, 50.0f, FColor::Cyan);
		DrawDebugCrosshairs(GetWorld(), NoiseLocation, FRotator(0.0f, 45.0f, 0.0f), 50.0f, FColor::Yellow);
	}

	if (AttackCooldownTimer > 0.0f)
	{
		AttackCooldownTimer -= DeltaTime;
	}

	if (BackupTimer > 0.0f)
	{
		BackupTimer -= DeltaTime;
	}
	
	if (State == EGuardState::Hunt && bCanSeePlayer)
	{
		if (BackupTimer <= 0.0f)
		{
			BackupTimer += GuardPawn->BackupCallInterval;
			UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetPawn()->GetActorLocation(), 1.0f, this, GuardPawn->BackupCallRange, FName("Backup"));
			GuardPawn->OnCallForBackup();
		}
	}
}

void AGuardAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	UE_LOG(LogTemp, Display, TEXT("AI possess"));

	GuardPawn = GetPawn<AGuardPawn>();
	if (GuardPawn == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Guard AI Controller not possessing a Guard Pawn!"));
		UnPossess();
		return;
	}

	OriginalRotation = GuardPawn->GetActorRotation();
	OriginalLocation = GuardPawn->GetActorLocation();
	BackupTimer = 0.0f;
	AttackCooldownTimer = 0.0f;

	RunBehaviorTree(BTAsset);
	if (GuardPawn->SightConfig == nullptr || GuardPawn->HearingConfig == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Guard perception configs are null! sight: %p, hearing: %p"), GuardPawn->SightConfig, GuardPawn->HearingConfig);
		UnPossess();
		return;
	}

	SetPerceptionComponent(*GuardPawn->PerceptionComponent);
	GuardPawn->ConfigureSenses();
	PerceptionComponent->ConfigureSense(*GuardPawn->SightConfig);
	PerceptionComponent->ConfigureSense(*GuardPawn->HearingConfig);
	PerceptionComponent->RequestStimuliListenerUpdate(); // This RequestUpdate is the one that seems to actually be needed for perception to work, but the one before ConfigureSense is needed to not get warnings...
	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AGuardAIController::OnTargetPerceptionUpdated);
	SetActorTickEnabled(true);
	Alertness = EAlertness::Neutral;

	// PlayerRef blackboard key is initialized in BeginPlay() because it's not available here
	GetBlackboardComponent()->SetValueAsFloat("LostTrackLookDuration", GuardPawn->LookAfterLosingPlayerDuration);
	GetBlackboardComponent()->SetValueAsFloat("LookBeforeInspectingDuration", GuardPawn->LookBeforeInspectingDuration);
}

void AGuardAIController::OnUnPossess()
{
	Super::OnUnPossess();

	GuardPawn = nullptr;
	PlayerRef = nullptr;
	SetActorTickEnabled(false);
	PerceptionComponent->OnTargetPerceptionUpdated.RemoveDynamic(this, &AGuardAIController::OnTargetPerceptionUpdated);
}

void AGuardAIController::Die()
{
	SetState(EGuardState::Dead);
	SetAlertness(EAlertness::Dead);
	SusValue = 0.0f;
	SetActorTickEnabled(false);
	PerceptionComponent->OnTargetPerceptionUpdated.RemoveDynamic(this, &AGuardAIController::OnTargetPerceptionUpdated);
}

AGuardPawn* AGuardAIController::GetGuardPawn() const
{
	return GuardPawn;
}

AGuardPatrolPath* AGuardAIController::GetPatrolPath() const
{
	if (GuardPawn == nullptr) return nullptr;
	return GuardPawn->PatrolPath;
}

FVector AGuardAIController::GetCurrentPatrolGoal() const
{
	AGuardPatrolPath* Path = GetPatrolPath();
	if (Path == nullptr)
	{
		return OriginalLocation;
	}
	
	USplineComponent* Spline = Path->GetSpline();
	return Spline->GetLocationAtSplinePoint(NextPatrolPoint, ESplineCoordinateSpace::World);
}

float AGuardAIController::GetAlertnessValue(EAlertness AlertnessLevel) const
{
	switch (AlertnessLevel)
	{
		case EAlertness::Neutral:
			return GuardPawn->SusNeutralRateMultiplier;
		case EAlertness::Distracted:
			return GuardPawn->SusDistractedRateMultiplier;
		case EAlertness::AlarmedKnowing:
		case EAlertness::AlarmedUnknowing:
			return GuardPawn->SusAlarmedRateMultiplier;
		default:
			return 1.0f;
	}
}

void AGuardAIController::PickNextPatrolPoint()
{
	AGuardPatrolPath* Path = GetPatrolPath();
	if (Path == nullptr) return;
	
	USplineComponent* Spline = Path->GetSpline();

	const int32 NumPoints = Spline->GetNumberOfSplinePoints();

	NextPatrolPoint += PatrolDirection;
	if (GetPatrolPath()->bPingPong && !Spline->IsClosedLoop())
	{
		if (NextPatrolPoint < 0 || NextPatrolPoint >= NumPoints)
		{
			PatrolDirection = -PatrolDirection;
			NextPatrolPoint += PatrolDirection * 2;
		}
	}

	NextPatrolPoint = (NextPatrolPoint % NumPoints + NumPoints) % NumPoints; // Actual modulo that handles negative numbers correctly
}

void AGuardAIController::FindClosestPatrolPoint()
{
	AGuardPatrolPath* Path = GetPatrolPath();
	if (Path == nullptr) return;

	NextPatrolPoint = Path->FindClosestPointToWorldLocation(GetPawn()->GetActorLocation());
	
}

FPatrolStop* AGuardAIController::GetCurrentPatrolStopInfo() const
{
	AGuardPatrolPath* Path = GetPatrolPath();
	if (Path == nullptr) return nullptr;
	for (FPatrolStop& Stop : Path->PatrolStops)
	{
		if (Stop.Where == NextPatrolPoint)
		{
			return &Stop;
		}
	}

	return nullptr;
}

bool AGuardAIController::IsSusEnough(ESusLevel Level) const
{
	switch (Level)
	{
	case ESusLevel::NotSus:
		return SusValue <= 0.0f;

	case ESusLevel::NonZero:
		return SusValue > 0.0f;
		
	case ESusLevel::KindaSus:
		return SusValue >= GuardPawn->SusLookThreshold;
		
	case ESusLevel::Inspect:
		return SusValue >= GuardPawn->SusInspectThreshold;
		
	case ESusLevel::Busted:
		return SusValue >= 1.0f;
		
	default:
		return false;
	}
}

void AGuardAIController::MakeThisOriginalRotation()
{
	OriginalRotation = GetControlRotation();
}

void AGuardAIController::ResetRotation()
{
	SetControlRotation(OriginalRotation);
}

// ReSharper disable once CppMemberFunctionMayBeConst
// Not logically const (or blueprint pure) since it modifies character's walk speed
void AGuardAIController::SetMovementSpeed(EMovementSpeed NewSpeed)
{
	float SpeedValue = 0.0f;
	if (NewSpeed == EMovementSpeed::Walk)
	{
		SpeedValue = GuardPawn->WalkSpeed;
	}
	else if (NewSpeed == EMovementSpeed::Run)
	{
		SpeedValue = GuardPawn->RunSpeed;
	}

	GuardPawn->GetCharacterMovement()->MaxWalkSpeed = SpeedValue;
}

void AGuardAIController::SetAlertness(EAlertness InAlertness)
{
	if (InAlertness <= EAlertness::Distracted && Alertness > EAlertness::Distracted)
	{
		// Cannot go from an alarmed state back to a careless state
	}
	else
	{
		Alertness = InAlertness;
	}

	if (InAlertness == EAlertness::AlarmedUnknowing)
	{
		// Bit of a hack, want to reset SusValue to 0% when completely losing track of chased player
		// in order to go directly back to patrol state (and not go through inspect or kinda sus states)
		GuardPawn->OnSusLevelDecreased(ESusLevel::NotSus, GetSusLevel());
		SusValue = 0.0f;
	}
}

void AGuardAIController::SetState(EGuardState InState)
{
	if (InState == EGuardState::Patrol && (State == EGuardState::Inspect || State == EGuardState::InspectLookAround))
	{
		GuardPawn->OnSusLevelDecreased(ESusLevel::NotSus, GetSusLevel());
		SusValue = 0.0f;
	}
	else if (InState == EGuardState::Inspect)
	{
		GetBlackboardComponent()->SetValueAsVector("InvestigationLocation", GetBlackboardComponent()->GetValueAsVector("NavigationGoalLocation"));
	}

	State = InState;
	
	if (InState > CurrentHighestState)
	{
		CurrentHighestState = InState;
	}
	else if (InState < CurrentHighestState)
	{
		EGuardState HighestState = (EGuardState)0;
		for (int32 i = 0; i < AllGuards.Num(); i++)
		{
			AGuardAIController* Guard = Cast<AGuardAIController>(AllGuards[i]);
			if (Guard != nullptr)
			{
				if (Guard->GetState() > HighestState)
				{
					HighestState = Guard->GetState();
				}
			}
		}

		CurrentHighestState = HighestState;
	}

}

void AGuardAIController::FindPathForMoveRequest(const FAIMoveRequest& MoveRequest, FPathFindingQuery& Query, FNavPathSharedPtr& OutPath) const
{
	Super::FindPathForMoveRequest(MoveRequest, Query, OutPath);
	FNavMeshPath* PathPtr = (FNavMeshPath*)OutPath.Get();
	if (PathPtr != nullptr)
	{
		PathPtr->OffsetFromCorners(GuardPawn->OffsetFromCornersDistance);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PathPtr is null!"));
	}
}

ESusLevel AGuardAIController::GetSusLevel() const
{
	if (SusValue >= 1.0f) return ESusLevel::Busted;
	if (SusValue >= GuardPawn->SusInspectThreshold) return ESusLevel::Inspect;
	if (SusValue >= GuardPawn->SusLookThreshold) return ESusLevel::KindaSus;
	if (SusValue > 0.0f) return ESusLevel::NonZero;
	return ESusLevel::NotSus;
}

bool AGuardAIController::IsVulnerableToStealthTakeDown() const
{
	return !bCanSeePlayer && Alertness != EAlertness::AlarmedKnowing;
}

bool AGuardAIController::CanAttackPlayer()
{
	if (PlayerRef == nullptr)
	{
		return false;
	}
	const float Distance = FVector::Dist2D(GetPawn()->GetActorLocation(), PlayerRef->GetActorLocation());
	const float ZDistance = FMath::Abs(GetPawn()->GetActorLocation().Z - PlayerRef->GetActorLocation().Z);
	const float MaxZDistance = GuardPawn->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	UE_LOG(LogTemp, Display, TEXT("Dist: %f, z: %f, cool: %f"), Distance, ZDistance, AttackCooldownTimer);
	if (Distance < GuardPawn->DistanceBeforeAttacking && ZDistance < MaxZDistance && AttackCooldownTimer <= 0.0f)
	{
		AttackCooldownTimer = GuardPawn->AttackCooldown;

		return true;
	}
	else
	{
		return false;
	}
}

EGuardState AGuardAIController::GetGlobalGuardAlertnessState()
{
	return CurrentHighestState;
}

void AGuardAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (GetPawn() == nullptr || GuardPawn == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("AI perception updated but not possesing anything, whatever will I do"));
		return;
	}
	
	UE_LOG(LogTemp, Display, TEXT("Perception update!"));
	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
	{
		UE_LOG(LogTemp, Display, TEXT("It's sight"));
		if (Actor->ActorHasTag(FName(TEXT("Player"))))
		{
			UE_LOG(LogTemp, Display, TEXT("'twas the player!"));
			bCanSeePlayer = Stimulus.WasSuccessfullySensed();
			GetBlackboardComponent()->SetValueAsBool(TEXT("HasLineOfSight"), bCanSeePlayer);
		}
	}
	else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
	{
		UE_LOG(LogTemp, Display, TEXT("It's noise"));
		const float Distance = FVector::Distance(Stimulus.StimulusLocation, GetPawn()->GetActorLocation());
		float DistanceFactor = FMath::Clamp(Distance / GuardPawn->HearingMaxRadius, 0.0f, 1.0f);

		FCollisionObjectQueryParams ObjectQueryParams(ECC_TO_BITFIELD(ECC_WorldStatic) | ECC_TO_BITFIELD(ECC_WorldDynamic));
		FCollisionQueryParams CollisionQueryParams(SCENE_QUERY_STAT(AILineOfSight), true, this);
		FHitResult HitResult;
		const bool bHit = GetWorld()->LineTraceSingleByObjectType(HitResult, Stimulus.StimulusLocation, Stimulus.ReceiverLocation, ObjectQueryParams, CollisionQueryParams);
	
		//if (bHit)
		//{
		//	UE_LOG(LogTemp, Display, TEXT("Occluded!"));
		//	DistanceFactor *= GuardPawn->HearingOcclusionDamp;
		//	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
		//	float PathLength;
		//	ENavigationQueryResult::Type Result = NavSys->GetPathLength(GetWorld(), Stimulus.StimulusLocation, Stimulus.ReceiverLocation, PathLength);
		//	if (Result == ENavigationQueryResult::Success)
		//	{
		//		UE_LOG(LogTemp, Display, TEXT("There is a path with length: %f"), PathLength);
		//		float PathFindDistanceFactor = FMath::Clamp(PathLength / GuardPawn->HearingMaxRadius, 0.0f, 1.0f);
		//		if (PathFindDistanceFactor < DistanceFactor)
		//		{
		//			UE_LOG(LogTemp, Display, TEXT("Pathfind was closer than going through wall"));
		//			DistanceFactor = PathFindDistanceFactor;
		//		}
		//	}
		//}
		
		if (Stimulus.Tag == FName(TEXT("Backup")))
		{
			if (Actor != this)
			{
				GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan, TEXT("I heard your request for backup, comrade!"));
				AGuardAIController* GuardInDistress = Cast<AGuardAIController>(Actor);
				SusValue = 1.0f;
				GetBlackboardComponent()->SetValueAsVector("NavigationGoalLocation", PlayerRef->GetActorLocation());
				const FVector PredictedLocation = PlayerRef->GetActorLocation() + PlayerRef->GetVelocity() * 100.0f;
				GetBlackboardComponent()->SetValueAsVector("PredictedPlayerLocation", PredictedLocation);
			}
		}
		else if (Alertness != EAlertness::AlarmedKnowing)
		{
			const bool InstantDistract = Stimulus.Tag == FName(TEXT("Noisemaker"));
			if (DistanceFactor >= GuardPawn->HearingFarThreshold && Alertness == EAlertness::Neutral && !InstantDistract)
			{
				// Ignore, noise wasn't suspicious enough
				return;
			}

			if (SusValue < GuardPawn->HearingMaxSus)
			{
				if (InstantDistract)
				{
					SusValue = GuardPawn->HearingMaxSus;
				}
				else
				{
					SusValue = FMath::Min(SusValue + (1.0f - DistanceFactor) * GuardPawn->HearingSusIncreaseMultiplier, GuardPawn->HearingMaxSus);
				}
			}

			if (State != EGuardState::Inspect && !IsSusEnough(ESusLevel::Busted))
			{
				GetBlackboardComponent()->SetValueAsVector("NavigationGoalLocation", Stimulus.StimulusLocation);
			}
		}
	}
}

FORCEINLINE bool AGuardAIController::ShouldShowDebug() const
{
#if UE_BUILD_SHIPPING
	return false;
#else
	return GuardPawn != nullptr && GuardPawn->bDebugInfo;
#endif
}
