// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Pawn.h"
#include "GuardPawn.generated.h"

class UGuardPawnMovementComponent;

UCLASS()
class ANACHRONIA_API AGuardPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGuardPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE UGuardPawnMovementComponent* GetGuardMovement() const { return MovementComponent; }

protected:
	UPROPERTY(Category = Guard, EditAnywhere, BlueprintReadWrite, Meta = (EditCondition = "MovementComponent && MovementComponent->PatrolPath"))
	bool bStartOnPath;
	
	UPROPERTY(Category = Guard, EditAnywhere, BlueprintReadWrite)
	float TurnSpeed = 500.0f;

	UPROPERTY(VisibleAnywhere)
	UGuardPawnMovementComponent* MovementComponent;

	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* CapsuleComponent;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* MeshComponent;
	
private:	
	virtual void OnConstruction(const FTransform& Transform) override;
};