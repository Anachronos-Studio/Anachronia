#include "BTService_CallForBackup.h"

#include "GuardAIController.h"
#include "GuardPawn.h"

UBTService_CallForBackup::UBTService_CallForBackup()
{
	NodeName = "Call for backup";
	RandomDeviation = 0.0f;
}

void UBTService_CallForBackup::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Yellow, TEXT("To arms, there's a thief!"));
}

void UBTService_CallForBackup::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AGuardPawn* Guard = Cast<AGuardAIController>(OwnerComp.GetAIOwner())->GetGuardPawn();
	Interval = Guard->BackupCallInterval;
	SoundMaxRange = Guard->BackupCallRange;
}
