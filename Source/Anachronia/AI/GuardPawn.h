// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Perception/AISightTargetInterface.h"

#include "GuardPawn.generated.h"

enum class ESusLevel : uint8;
class UAISenseConfig_Hearing;
class AGuardAIController;
class UAISenseConfig_Sight;
class UAIPerceptionComponent;
class AGuardPatrolPath;
class UGuardPawnMovementComponent;

UCLASS()
class ANACHRONIA_API AGuardPawn : public ACharacter, public IAISightTargetInterface
{
private:
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGuardPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Guard")
	void OnDeath(bool bNonLethalDeath);

	UFUNCTION(BlueprintImplementableEvent, Category = "Guard")
	void OnDamaged(float NewHealth, float OldHealth);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Guard")
	void OnSusLevelIncreased(ESusLevel NewSusLevel, ESusLevel OldSusLevel);

	UFUNCTION(BlueprintImplementableEvent, Category = "Guard")
	void OnSusLevelDecreased(ESusLevel NewSusLevel, ESusLevel OldSusLevel);

	UFUNCTION(BlueprintImplementableEvent, Category = "Guard")
	void OnCallForBackup();

	UFUNCTION(BlueprintImplementableEvent, Category = "Guard")
	void OnAttack();

	UFUNCTION(BlueprintImplementableEvent, Category = "Guard")
	void OnSawDeadGuardBody();

	UFUNCTION(BlueprintImplementableEvent, Category = "Guard")
	void OnInspectDeadGuardBody();

	void Respawn();
	
	virtual bool CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor) const override;

	UFUNCTION(BlueprintCallable, Category = "Guard")
	void SetDamageToCurrentHealth(float Damage, bool bNonLethal);
	
	UPROPERTY(Category = "Guard", EditInstanceOnly)
	AGuardPatrolPath* PatrolPath;
	
	/* If true, will be immediately moved to the closest node on the patrol path
	 * when the game starts (and rotated to face in proper direction)
	 */
	UPROPERTY(Category = "Guard", EditAnywhere)
	bool bStartOnPath;

	UPROPERTY(Category = "Guard", EditAnywhere)
	bool bDebugInfo = false;

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

	/*
	 * If player's visibility is below this threshold, guard can never see player at all (unless she's very close)
	 */
	UPROPERTY(Category = "Guard|Sight perception", EditAnywhere, BlueprintReadWrite)
	float PlayerMinVisibility = 0.1f;

	// If player is at least this close to guard, guard can always see player (regardless of their visibility)
	UPROPERTY(Category = "Guard|Sight perception", EditAnywhere, BlueprintReadWrite)
	float AlwaysSeeDistance = 100.0f;

	// How much faster to fill sus meter when player is within AlwaysSeeDistance
	UPROPERTY(Category = "Guard|Sight perception", EditAnywhere, BlueprintReadWrite)
	float VeryCloseSusIncreaseMultiplier = 10.0f;
	
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

	UPROPERTY(Category = "Guard|Sight perception", EditAnywhere, BlueprintReadWrite)
	float BodyCanBeSeenFromRange = 2000.0f;
	
	/* *****************************
	 * Hearing perception
	 */

	// When distance to sound is smaller than this value multiplied by the sound range, the sound is considered to have been very close
	UPROPERTY(Category = "Guard|Hearing perception", EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 1))
	float HearingCloseThreshold = 0.1f;

	// When distance to sound is larger or equal to this value multiplied by the sound range, the sound is considered to have been very far away
	UPROPERTY(Category = "Guard|Hearing perception", EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 1))
	float HearingFarThreshold = 0.7f;

	// Maximum SusValue a sound can result in
	UPROPERTY(Category = "Guard|Hearing perception", EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 1))
	float HearingMaxSus = 0.8f;

	// When adding to the SusValue because of a sound, the increase (based on distance) is multiplied with this factor
	UPROPERTY(Category = "Guard|Hearing perception", EditAnywhere, BlueprintReadWrite)
	float HearingSusIncreaseMultiplier = 0.5f;

	// How much to multiply sound loudness (effectively its range) when occluded by one obstacle
	UPROPERTY(Category = "Guard|Hearing perception", EditAnywhere, BlueprintReadWrite)
	float HearingOcclusionDamp = 0.5f;

	// How much to multiply remaining sound loudness with when occluded by a second obstacle
	UPROPERTY(Category = "Guard|Hearing perception", EditAnywhere, BlueprintReadWrite)
	float HearingDoubleOcclusionDamp = 0.0f;
	
	/* *****************************
	 * Call for backup
	 */

	// How long (in seconds) between each time the guard calls for backup when hunting player
	UPROPERTY(Category = "Guard|Call for backup", EditAnywhere, BlueprintReadWrite)
	float BackupCallInterval = 10.0f;

	// Range of sound made when calling for backup
	UPROPERTY(Category = "Guard|Call for backup", EditAnywhere, BlueprintReadWrite)
	float BackupCallRange = 1000.0f;
	
	UPROPERTY(VisibleAnywhere, Transient)
	UAISenseConfig_Sight* SightConfig;

	/**********************
	 * Combat
	 */

	UPROPERTY(Category = "Guard|Combat", EditAnywhere, BlueprintReadWrite)
	float MaxHealth = 100.0f;

	UPROPERTY(Category = "Guard|Combat", Transient, VisibleAnywhere, BlueprintReadWrite)
	float CurrentHealth = 100.0f;

	// How much damage to deal to player per blow
	UPROPERTY(Category = "Guard|Combat", EditAnywhere, BlueprintReadWrite)
	float AttackDamage = 25.0f;

	// How close to the player does the guard need to be to try attacking?
	UPROPERTY(Category = "Guard|Combat", EditAnywhere, BlueprintReadWrite)
	float DistanceBeforeAttacking = 100.0f;

	// How many seconds between attacks (minimum)?
	UPROPERTY(Category = "Guard|Combat", EditAnywhere, BlueprintReadWrite)
	float AttackCooldown = 0.75f;

	UPROPERTY(VisibleAnywhere)
	UAIPerceptionComponent* PerceptionComponent;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	AGuardAIController* GetGuardAI() const;

	virtual void GetActorEyesViewPoint(FVector& Location, FRotator& Rotation) const override;
	void ConfigureSenses();
};