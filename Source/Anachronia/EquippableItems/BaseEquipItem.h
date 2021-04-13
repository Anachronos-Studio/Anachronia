// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BaseEquipItem.generated.h"

UCLASS()
class ANACHRONIA_API ABaseEquipItem : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABaseEquipItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Action();

	virtual void ChargeWeapon();

	virtual void UseTool();

protected:
	// Checks if the equippable item is a weapon or not
	bool bIsWeapon;
};
