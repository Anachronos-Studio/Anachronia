// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Volume.h"
#include "RoomVolume.generated.h"

class AGuardPatrolPath;
/**
 * 
 */
UCLASS()
class ANACHRONIA_API ARoomVolume : public AVolume
{
	GENERATED_BODY()

public:
	ARoomVolume();
	
	void ClaimPath(AGuardPatrolPath* Path);
	void AbandonPath(AGuardPatrolPath* Path);
	AGuardPatrolPath* FindFreePath();
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	TArray<AGuardPatrolPath*> PatrolPaths;

	UPROPERTY()
	TArray<AGuardPatrolPath*> FreePaths;
	
public:
#if WITH_EDITOR
	virtual void OnConstruction(const FTransform& Transform) override;
#endif // WITH_EDITOR

};