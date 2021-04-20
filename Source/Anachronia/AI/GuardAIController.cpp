#include "GuardAIController.h"

#include "GuardPatrolPath.h"
#include "GuardPawn.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISenseConfig_Sight.h"

AGuardAIController::AGuardAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	bSetControlRotationFromPawnOrientation = false;
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BehaviorTreeAsset(TEXT("/Game/Anachronia/BehaviorTrees/BT_Guard"));
	if (BehaviorTreeAsset.Succeeded())
	{
		BTAsset = BehaviorTreeAsset.Object;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load guard behavior tree asset"));
	}
}

void AGuardAIController::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Display, TEXT("AI play"));
}

void AGuardAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bCanSeePlayer)
	{
		const float Rate = GuardPawn->SusBaseIncreaseRate; // TODO: Take in rate multiplier from player
		SusValue = FMath::Min(SusValue + Rate * DeltaTime, 1.0f);
		if (IsSusEnough(ESusLevel::KindaSus))
		{
			GetBlackboardComponent()->SetValueAsVector(TEXT("LookLocation"), PlayerRef->GetActorLocation());
		}
	}
	else
	{
		SusValue = FMath::Max(SusValue - GuardPawn->SusDecreaseRate * DeltaTime, 0.0f);
	}

	FString Msg = FString::Printf(TEXT("Sus: %3.0f%% [%s]"),
		SusValue * 100.0f,
		*UEnum::GetValueAsString<ESusLevel>(GetSusLevel())
	);
	GEngine->AddOnScreenDebugMessage(419, 1.0f, FColor::White, Msg);
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
		return GuardPawn->GetActorLocation();
	}
	
	USplineComponent* Spline = Path->GetSpline();
	return Spline->GetLocationAtSplinePoint(NextPatrolPoint, ESplineCoordinateSpace::World);
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

ESusLevel AGuardAIController::GetSusLevel() const
{
	if (SusValue >= 1.0f) return ESusLevel::Busted;
	if (SusValue >= GuardPawn->SusInspectThreshold) return ESusLevel::Inspect;
	if (SusValue >= GuardPawn->SusLookThreshold) return ESusLevel::KindaSus;
	if (SusValue > 0.0f) return ESusLevel::NonZero;
	return ESusLevel::NotSus;
}

void AGuardAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	UE_LOG(LogTemp, Display, TEXT("AI possess"));

	GuardPawn = GetPawn<AGuardPawn>();
	if (GuardPawn == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Guard AI Controller not possessing a Guard Pawn!"));
		return;
	}

	OriginalRotation = GuardPawn->GetActorRotation();
	
	RunBehaviorTree(BTAsset);
	SetPerceptionComponent(*GuardPawn->PerceptionComponent);
	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AGuardAIController::OnTargetPerceptionUpdated);
}

void AGuardAIController::OnUnPossess()
{
	Super::OnUnPossess();

	GuardPawn = nullptr;
	PlayerRef = nullptr;
}

void AGuardAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
	{
		if (Actor->ActorHasTag(FName(TEXT("Player"))))
		{
			PlayerRef = Actor;
			bCanSeePlayer = Stimulus.WasSuccessfullySensed();
		}
	}
}
