// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/AISightTargetInterface.h"
#include "Templates/UnrealTemplate.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "AnachroniaPlayer.generated.h"

class ABaseEquipItem;
class UAchievement;
class UAnachroniaSaveGame;
UCLASS()
class ANACHRONIA_API AAnachroniaPlayer : public ACharacter, public IAISightTargetInterface
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LightDetector)
	class ALightDetector* PlayerLightDetector;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EquippableItems)
	TArray<ABaseEquipItem*> EquippableItems;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EquippableItems)
	class UChildActorComponent* EquippableItem;



	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LightDetector)
	//class UChildActorComponent* Detector;


	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = LightDetector)
	//UCameraComponent* LightCamTop;
	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = LightDetector)
	//UCameraComponent* LightCamBottom;

	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = LightDetector)
	//class USceneCaptureComponent2D* SceneCaptureTop;
	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = LightDetector)
	//USceneCaptureComponent2D* SceneCaptureBottom;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LightDetector)
	//UTextureRenderTarget2D* RenderTargetTop;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LightDetector)
	//UTextureRenderTarget2D* RenderTargetBottom;


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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MenuOptions)
	bool bTogglePlayerCrouch;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = BasicAttributes)
	float JumpVelocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = BasicAttributes)
	bool bPlayerIsCrouched;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = BasicAttributes)
	bool bPlayerIsSprinting;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = BasicAttributes)
	float PlayerCapsuleStandingHalfHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BasicAttributes)
	float PlayerCapsuleCrouchedHalfHeight;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = BasicAttributes)
	FVector RelativeCamLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BasicAttributes)
	FVector RelativeCamCrouchedLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BasicAttributes)
	float PlayerSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BasicAttributes)
	float PlayerCrouchedSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BasicAttributes)
	bool bCannotMove = false;

	// Stealth properties below

	/** This value will determine the overall visibility of the player to the AI */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = StealthLevelAttributes)
	float PlayerVisibility;

	/** The Luminance value will handle the visual luminosity of the player */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = StealthLevelAttributes)
	float PlayerLuminance;

	/** The Noise level is dependent on player movement types as well as floor material*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = StealthLevelAttributes)
	float PlayerNoiseLevel;																				//Separera från visibility

	/** The player motion level determines how much motion player makes, to get noticeable */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = StealthLevelAttributes)
	float PlayerMotionLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LightDetector)
	float LightDetectorLevel;

	/** Min value for the Player Visibility */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = StealthLevelAttributes)
	float LuminanceMinValue;


	// Player Main Attributes below

	/** The maximum health of the player*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MainAttributes)
	float MaxHealth;

	/** The current health of the player*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MainAttributes)
	float CurrentHealth;

	/** The Basic damage player can do, affects both lethal damage and blunt damage by addition */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MainAttributes)
	float BaseDamage;

	/** The lethal damage that will get it's value from weapons */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MainAttributes)
	float LethalDamage;

	/** The blunt damage that will get it's value from weapons */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MainAttributes)
	float BluntDamage;

	// Save game data related below
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoring & Achievements")
	int32 PlayerScore;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoring & Achievements")
	int32 ImperialMarks;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Scoring & Achievements")
	TArray<FName> PlayerReadBooksNames;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoring & Achievements")
	//TMap<FName, TSubclassOf<UAchievement>> PlayerAchievementsMap;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoring & Achievements")
	//TMap<FName, UAchievement*> PlayerAchievementMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoring & Achievements")
	class UAchievementContainer* AchievementContainer = CreateDefaultSubobject<UAchievementContainer>(TEXT("AchievementContainer"));
	

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

	void CheckLights();
	FColor& FindBrightestPixel(TArray<FColor>& ColorBuffer, int32 start, int32 max);

	/** Calculate the luminosity. This function should be called when setting up player visibility check */
	float CalculateLuminance(const FVector& V);

	UPROPERTY(BlueprintReadOnly)
	FVector GlobalLuminanceOnPlayer;

	float DeltaTimeTimer = 0.f;

	

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
	void SetVisibility(float L, float M, float LumMinValue) {
		float Temp;
		if(L <= LumMinValue)
			L = LumMinValue;
		Temp = L * M;
		PlayerVisibility = Temp; 
	}
	//void SetVisibility(float L, float M) { PlayerVisibility = (L + M) / 2.f; }

	UFUNCTION(BlueprintCallable, Category = StealthAttributes)
	float GetLuminance() { return PlayerLuminance; }
	UFUNCTION(BlueprintCallable, Category = StealthAttributes)
	float GetMotionLevel() { return PlayerMotionLevel; }
	UFUNCTION(BlueprintCallable, Category = StealthAttributes)
	float GetVisibility() { return PlayerVisibility; }

	UFUNCTION(BlueprintCallable, Category = StealthAttributes)
	void SetGlobalLuminanceOnPlayer(FVector L) { GlobalLuminanceOnPlayer = L; }

	UFUNCTION(BlueprintCallable, Category = StealthAttributes)
	void SetPlayerNoiseLevel(float Value) { PlayerNoiseLevel = Value; }
	UFUNCTION(BlueprintCallable, Category = StealthAttributes)
	float GetPlayerNoiseLevel() { return PlayerNoiseLevel; }


	virtual bool CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation,
	int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor) const override;

	UFUNCTION(BlueprintCallable)
	void SaveGame();
	UFUNCTION(BlueprintCallable)
	void LoadGame();

	/** Set the achievement to true*/
	//UFUNCTION(BlueprintCallable)
	//void ActivateAchievement(FName Name);
	/** Use this to set the  achievement false*/
	//UFUNCTION(BlueprintCallable)
	//void DisableAchievement(FName Name);

	//UFUNCTION(BlueprintCallable)
	//void SaveAchievementsStatus(UAnachroniaSaveGame* SaveGameInstance);
	//UFUNCTION(BlueprintCallable)
	//void LoadAchievementsStatus(UAnachroniaSaveGame* LoadGameInstance);

	//void InitiateAchievements(UAnachroniaSaveGame* SaveGameInstance);
};
