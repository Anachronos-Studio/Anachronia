// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AnachroniaPlayer.generated.h"

UCLASS()
class ANACHRONIA_API AAnachroniaPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAnachroniaPlayer();

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
	class USkeletalMeshComponent* Mesh1P;

	//Insert component for tool/weapon

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	/** To catch the global lighting to determine player visibility */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
	class UStaticMeshComponent* LightReceiver;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = BasicAttributes)
	float JumpVelocity;

	// Stealth properties below

	/** This value will determine the overall visibility of the player to the AI */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StealthLevelAttributes)
	float PlayerVisibility;

	/** The Luminance value will handle the visual luminosity of the player */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StealthLevelAttributes)
	float PlayerLuminance;

	/** The Noise level is dependent on player movement types as well as floor material*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StealthLevelAttributes)
	float PlayerNoiseLevel;

	/** The player motion level determines how much motion player makes, to get noticeable */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StealthLevelAttributes)
	float PlayerMotionLevel;

protected:

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	void ToggleCrouch();
	bool bIsCrouched;

	/** Calculate the luminosity. This function should be called when setting up player visibility check */
	float CalculateLuminance(FVector V);



public:
	UFUNCTION(BlueprintCallable, Category = BasicAttributes)
	void ToggleCrouchOn();

	UFUNCTION(BlueprintCallable, Category = BasicAttributes)
	void ToggleCrouchOff();

};
