#include "GuardAIController.h"

#include "GuardPatrolPath.h"
#include "GuardPawn.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Components/SplineComponent.h"

AGuardAIController::AGuardAIController()
{
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

void AGuardAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	GuardPawn = GetPawn<AGuardPawn>();
	if (GuardPawn == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Guard AI Controller not possessing a Guard Pawn!"));
		return;
	}

	RunBehaviorTree(BTAsset);
}

void AGuardAIController::OnUnPossess()
{
	Super::OnUnPossess();

	GuardPawn = nullptr;
}
