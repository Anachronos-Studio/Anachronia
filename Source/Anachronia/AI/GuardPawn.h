// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GuardPawn.generated.h"

class AGuardAIController;
class UAISenseConfig_Sight;
class UAIPerceptionComponent;
class AGuardPatrolPath;
class UGuardPawnMovementComponent;

UCLASS()
class ANACHRONIA_API AGuardPawn : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGuardPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(Category = Guard, EditInstanceOnly)
	AGuardPatrolPath* PatrolPath;
	
	/* If true, will be immediately moved to the closest node on the patrol path
	 * when the game starts (and rotated to face in proper direction)
	 */
	UPROPERTY(Category = Guard, EditAnywhere)
	bool bStartOnPath;

	/* *********************
	 * Movement
	 */
	
	UPROPERTY(Category = "Guard|Movement", EditAnywhere, BlueprintReadWrite)
	float WalkSpeed = 200.0f;

	UPROPERTY(Category = "Guard|Movement", EditAnywhere, BlueprintReadWrite)
	float RunSpeed = 400.0f;

	// Makes guards not hug walls and corners too closely when pathfinding
	UPROPERTY(Category = "Guard|Movement", EditAnywhere, BlueprintReadWrite)
	float OffsetFromCornersDistance = 100.0f;

	// When the guard has noticed something suspicious and is about to inspect it, they
	// will first look towards the source for this many seconds
	UPROPERTY(Category = "Guard|Movement", EditAnywhere, BlueprintReadWrite)
	float LookBeforeInspectingDuration = 2.0f;

	// When inspecting a suspicious thing, the guard will look around for this long in each direction
	// before deciding it must have been the wind
	UPROPERTY(Category = "Guard|Movement", EditAnywhere, BlueprintReadWrite)
	float LookAroundWhenInspectingDuration = 1.0f;
	
	// After losing track of player and reaching her last known location
	// this determines how long the guard stays there and looks before returning to patrol path
	UPROPERTY(Category = "Guard|Movement", EditAnywhere, BlueprintReadWrite)
	float LookAfterLosingPlayerDuration = 2.0f;

	/* ************************
	 * General perception
	 */
	
	// When SusValue exceeds this, guard will stop and look towards the suspicious thing
	UPROPERTY(Category = "Guard|General perception", EditAnywhere, BlueprintReadWrite)
	float SusLookThreshold = 0.3f;

	// When SusValue exceeds this, guard will walk towards the suspicious thing to look closer
	UPROPERTY(Category = "Guard|General perception", EditAnywhere, BlueprintReadWrite)
	float SusInspectThreshold = 0.6f;

	// How much to add to SusValue per second (rate is first multiplied by other parameters)
	UPROPERTY(Category = "Guard|General perception", EditAnywhere, BlueprintReadWrite)
	float SusBaseIncreaseRate = 1.0f;

	// How much to subtract from SusValue per second when nothing is increasing it
	UPROPERTY(Category = "Guard|General perception", EditAnywhere, BlueprintReadWrite)
	float SusDecreaseRate = 0.25f;

	/* *************************
	 * Sight perception
	 */
	
	// Maximum sight distance to notice player. Changes corresponding parameter in PerceptionComponent.
	UPROPERTY(Category = "Guard|Sight perception", EditAnywhere, BlueprintReadWrite)
	float SightRadius = 3000.0f;

	// Maximum sight distance to see player that has been already seen. Changes corresponding parameter in PerceptionComponent.
	UPROPERTY(Category = "Guard|Sight perception", EditAnywhere, BlueprintReadWrite)
	float LoseSightRadius = 3500.0f;

	/** How far to the side AI can see, in degrees.
	 *	The value represents the angle measured in relation to the forward vector, not the whole range.
	 *	Changes corresponding parameter in PerceptionComponent.
	 */
	UPROPERTY(Category = "Guard|Sight perception", EditAnywhere, BlueprintReadWrite)
	float PeripheralVisionHalfAngle = 90.f;

	// SusValue increase rate multiplier when in an alarmed alertness state
	UPROPERTY(Category = "Guard|Sight perception", EditAnywhere, BlueprintReadWrite)
	float SusAlarmedRateMultiplier = 1.0f;

	// SusValue increase rate multiplier when in distracted alertness state
	UPROPERTY(Category = "Guard|Sight perception", EditAnywhere, BlueprintReadWrite)
	float SusDistractedRateMultiplier = 0.75f;

	// SusValue increase rate multiplier when in neutral alertness state
	UPROPERTY(Category = "Guard|Sight perception", EditAnywhere, BlueprintReadWrite)
	float SusNeutralRateMultiplier = 0.5f;

	// How much distance to player affects SusValue increase rate. Distance is 0-1 where 0 is max distance away (SightRadius).
	// That distance value is then multiplied by this parameter.
	UPROPERTY(Category = "Guard|Sight perception", EditAnywhere, BlueprintReadWrite)
	float SusDistanceRateMultiplier = 1.0f;

	UPROPERTY(VisibleAnywhere)
	UAIPerceptionComponent* PerceptionComponent;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	UFUNCTION(BlueprintCallable)
	AGuardAIController* GetGuardAI() const;
};