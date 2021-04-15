// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GuardPawn.generated.h"

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
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	virtual void OnConstruction(const FTransform& Transform) override;
};