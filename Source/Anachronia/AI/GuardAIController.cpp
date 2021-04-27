#include "GuardAIController.h"

#include "GuardPatrolPath.h"
#include "GuardPawn.h"
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

	if (GuardPawn == nullptr)
	{
		return;
	}
	
	GetBlackboardComponent()->SetValueAsObject("Player", PlayerRef);
	GetBlackboardComponent()->SetValueAsFloat("LostTrackLookDuration", GuardPawn->LookAfterLosingPlayerDuration);
	GetBlackboardComponent()->SetValueAsFloat("LookBeforeInspectingDuration", GuardPawn->LookBeforeInspectingDuration);
}

void AGuardAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bCanSeePlayer)
	{
		float Rate = GuardPawn->SusBaseIncreaseRate;
		Rate *= GetAlertnessValue(Alertness);
		Rate *= PlayerRef->GetVisibility();
		const float Distance = FVector::Dist(GuardPawn->GetActorLocation(), PlayerRef->GetActorLocation());
		const float DistanceFactor = 1.0f - FMath::Clamp(Distance / GuardPawn->SightRadius, 0.0f, 1.0f);
		Rate *= DistanceFactor * GuardPawn->SusDistanceRateMultiplier;
		
		const FString Msg = FString::Printf(TEXT("Rate: %.3f\nDist: %.3f"),
			Rate,
			DistanceFactor
		);
		GEngine->AddOnScreenDebugMessage(420, 1.0f, FColor::White, Msg);

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
	else
	{
		float MinSus = 0.0f;
		if (Alertness == EAlertness::AlarmedKnowing)
		{
			MinSus = 1.0f;
		}
		else if (State == EGuardState::Inspect)
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

	const FString Msg = FString::Printf(TEXT("Sus: %3.0f%% [%s]\nAlertness: %s\nMain state: %s\n%s"),
		SusValue * 100.0f,
		*StaticEnum<ESusLevel>()->GetValueAsString(GetSusLevel()),
		*StaticEnum<EAlertness>()->GetValueAsString(Alertness),
		*StaticEnum<EGuardState>()->GetValueAsString(State),
		bCanSeePlayer ? TEXT("Player in sight") : TEXT("Can't see player")
	);
	GEngine->AddOnScreenDebugMessage(419, 1.0f, FColor::White, Msg);

	if (AttackCooldownTimer > 0.0f)
	{
		AttackCooldownTimer -= DeltaTime;
	}
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
	if (InState == EGuardState::Patrol && State == EGuardState::Inspect)
	{
		GuardPawn->OnSusLevelDecreased(ESusLevel::NotSus, GetSusLevel());
		SusValue = 0.0f;
	}
	State = InState;
}

void AGuardAIController::FindPathForMoveRequest(const FAIMoveRequest& MoveRequest, FPathFindingQuery& Query, FNavPathSharedPtr& OutPath) const
{
	Super::FindPathForMoveRequest(MoveRequest, Query, OutPath);
	((FNavMeshPath*)OutPath.Get())->OffsetFromCorners(GuardPawn->OffsetFromCornersDistance);
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
	
	RunBehaviorTree(BTAsset);
	if (GuardPawn->SightConfig == nullptr || GuardPawn->HearingConfig == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Guard perception configs are null! sight: %p, hearing: %p"), GuardPawn->SightConfig, GuardPawn->HearingConfig);
		UnPossess();
		return;
	}
	
	SetPerceptionComponent(*GuardPawn->PerceptionComponent);
	PerceptionComponent->RequestStimuliListenerUpdate();
	PerceptionComponent->ConfigureSense(*GuardPawn->SightConfig);
	PerceptionComponent->ConfigureSense(*GuardPawn->HearingConfig);
	PerceptionComponent->RequestStimuliListenerUpdate(); // This RequestUpdate is the one that seems to actually be needed for perception to work, but the one before ConfigureSense is needed to not get warnings...
	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AGuardAIController::OnTargetPerceptionUpdated);
	SetActorTickEnabled(true);
	Alertness = EAlertness::Neutral;
}

void AGuardAIController::OnUnPossess()
{
	Super::OnUnPossess();

	GuardPawn = nullptr;
	PlayerRef = nullptr;
	SetActorTickEnabled(false);
}

void AGuardAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
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
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("Guard heard something!!!!"));
	}
}
