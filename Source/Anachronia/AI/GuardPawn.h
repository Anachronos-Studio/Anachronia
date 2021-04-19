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
	 * when the game starts
	 */
	UPROPERTY(Category = Guard, EditAnywhere)
	bool bStartOnPath;

	UPROPERTY(Category = "Guard|Perception", EditAnywhere, BlueprintReadWrite)
	float SusLookThreshold = 0.3f;

	UPROPERTY(Category = "Guard|Perception", EditAnywhere, BlueprintReadWrite)
	float SusInspectThreshold = 0.6f;

	UPROPERTY(Category = "Guard|Perception", EditAnywhere, BlueprintReadWrite)
	float SusBaseIncreaseRate = 0.5f;

	UPROPERTY(Category = "Guard|Perception", EditAnywhere, BlueprintReadWrite)
	float SusDecreaseRate = 0.25f;

	// Maximum sight distance to notice player. Changes corresponding parameter in PerceptionComponent.
	UPROPERTY(Category = "Guard|Perception", EditAnywhere, BlueprintReadWrite)
	float SightRadius = 3000.0f;

	// Maximum sight distance to see player that has been already seen. Changes corresponding parameter in PerceptionComponent.
	UPROPERTY(Category = "Guard|Perception", EditAnywhere, BlueprintReadWrite)
	float LoseSightRadius = 3500.0f;

	UPROPERTY(VisibleAnywhere)
	UAIPerceptionComponent* PerceptionComponent;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	UFUNCTION(BlueprintCallable)
	AGuardAIController* GetGuardAI() const;
};