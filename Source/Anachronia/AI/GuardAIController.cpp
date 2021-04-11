#include "GuardAIController.h"
#include "GuardPawn.h"
#include "BehaviorTree/BehaviorTree.h"

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

	UE_LOG(LogTemp, Display, TEXT("Beging play"));
}

AGuardPawn* AGuardAIController::GetGuardPawn() const
{
	return GuardPawn;
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
