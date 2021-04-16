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
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = LightDetector)
	class UStaticMeshComponent* LightReceiver;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = LightDetector)
	UCameraComponent* LightCamTop;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = LightDetector)
	UCameraComponent* LightCamBottom;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = BasicAttributes)
	bool bPlayerIsCrouched;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = BasicAttributes)
	float PlayerCapsuleStandingHalfHeight;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = BasicAttributes)
	float PlayerCapsuleCrouchedHalfHeight;

	// Stealth properties below

	/** This value will determine the overall visibility of the player to the AI */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StealthLevelAttributes)
	float PlayerVisibility;

	/** The Luminance value will handle the visual luminosity of the player */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StealthLevelAttributes)
	float PlayerLuminance;

	/** The Noise level is dependent on player movement types as well as floor material*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StealthLevelAttributes)
	float PlayerNoiseLevel;																				//Separera från visibility

	/** The player motion level determines how much motion player makes, to get noticeable */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StealthLevelAttributes)
	float PlayerMotionLevel;

	// Player Main Attributes below

	/** The maximum health of the player*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MainAttributes)
	float MaxHealth;

	/** The current health of the player*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MainAttributes)
	float CurrentHealth;

	/** The Basic damage player can do, affects both lethal damage and blunt damage by addition */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MainAttributes)
	float BaseDamage;

	/** The lethal damage that will get it's value from weapons */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MainAttributes)
	float LethalDamage;

	/** The blunt damage that will get it's value from weapons */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MainAttributes)
	float BluntDamage;		

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

	void Sprint();
	void UnSprint();
	float InitiatedWalkingSpeed;

	void ToggleCrouch();
	

	/** Calculate the luminosity. This function should be called when setting up player visibility check */
	float CalculateLuminance(const FVector &V);

	UPROPERTY(BlueprintReadOnly)
	FVector GloabalLuminanceOnPlayer;
	

public:
	UFUNCTION(BlueprintCallable, Category = BasicAttributes)
	void ToggleCrouchOn();

	UFUNCTION(BlueprintCallable, Category = BasicAttributes)
	void ToggleCrouchOff();

	// Main attributes functions
	UFUNCTION(BlueprintCallable, Category = MainAttributes)
	void SetCurrentHealth(float Value) { CurrentHealth = Value; }
	UFUNCTION(BlueprintCallable, Category = MainAttributes)
	void SetDamageToPlayerCurrentHealth(float DamageValue) { CurrentHealth -= DamageValue; }
	UFUNCTION(BlueprintCallable, Category = MainAttributes)
	float GetCurrentHealth() { return CurrentHealth; }
	UFUNCTION(BlueprintCallable, Category = MainAttributes)
	void SetMaxHealth(float Value) { MaxHealth = Value; }
	UFUNCTION(BlueprintCallable, Category = MainAttributes)
	float GetMaxHealth() { return MaxHealth; }

	UFUNCTION(BlueprintCallable, Category = MainAttributes)
	void SetBaseDamage(float Value) { BaseDamage = Value; }
	UFUNCTION(BlueprintCallable, Category = MainAttributes)
	float GetBaseDamage() { return BaseDamage; }

	UFUNCTION(BlueprintCallable, Category = MainAttributes)
	void SetLethalDamage(float PlayerBaseDamage, float WeaponLeathalDamage) { 
		LethalDamage = PlayerBaseDamage + WeaponLeathalDamage;
	}
	UFUNCTION(BlueprintCallable, Category = MainAttributes)
	float GetLethalDamage() { return LethalDamage; }

	UFUNCTION(BlueprintCallable, Category = MainAttributes)
	void SetBluntDamage(float PlayerBaseDamage, float WeaponBluntDamage) {
		BluntDamage = PlayerBaseDamage + WeaponBluntDamage;
	}
	UFUNCTION(BlueprintCallable, Category = MainAttributes)
	float GetBluntDamage() { return BluntDamage; }


	// Stealth attributes functions
	UFUNCTION(BlueprintCallable, Category = StealthAttributes)
	void SetLuminance(float Value) { PlayerLuminance = Value; }
	UFUNCTION(BlueprintCallable, Category = StealthAttributes)
	void SetMotionLevel(float Value) { PlayerMotionLevel = Value; }
	UFUNCTION(BlueprintCallable, Category = StealthAttributes)
	void SetVisibility(float L, float M) { PlayerVisibility = (L + M) / 2.f; }

	UFUNCTION(BlueprintCallable, Category = StealthAttributes)
	float GetLuminance(){ return PlayerLuminance; }
	UFUNCTION(BlueprintCallable, Category = StealthAttributes)
	float GetMotionLevel(){ return PlayerMotionLevel; }
	UFUNCTION(BlueprintCallable, Category = StealthAttributes)
	float GetVisibility(){ return PlayerVisibility; }

	UFUNCTION(BlueprintCallable, Category = StealthAttributes)
	void SetGlobalLuminanceOnPlayer(FVector L){ GloabalLuminanceOnPlayer = L; }
	
};
