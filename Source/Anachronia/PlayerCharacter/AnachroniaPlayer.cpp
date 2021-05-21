// Fill out your copyright notice in the Description page of Project Settings.


#include "AnachroniaPlayer.h"

#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DirectionalLight.h"
#include "LightDetector.h"
#include "Components/ChildActorComponent.h"
#include "../EquippableItems/BaseEquipItem.h"
#include "../Utility/AnachroniaSaveGame.h"
#include "../Utility/Achievement.h"
#include "../Utility/AchievementContainer.h"


// Sets default values
AAnachroniaPlayer::AAnachroniaPlayer()
{	
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
	//AchievementContainer = CreateDefaultSubobject<UAchievementContainer>(TEXT("AchievementContainer"));

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	Tags.Add(TEXT("Player"));

	// Initiate Menu Options
	bTogglePlayerCrouch = true;

	// Jump velocity that sets the height of the player jump
	JumpVelocity = 330.f;

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;


	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	EquippableItem = CreateDefaultSubobject<UChildActorComponent>(TEXT("EquippableItem"));
	EquippableItem->SetupAttachment(FirstPersonCameraComponent);

	// Create a box that will catch the lighing
	LightReceiver = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LightReceiver"));
	LightReceiver->SetupAttachment(GetCapsuleComponent());
	LightReceiver->SetRelativeLocation(FVector(0.f, 1.75f, 0.f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Octahedron(TEXT("StaticMesh'/Game/Anachronia/Meshes/Oct.Oct'"));
	if (Octahedron.Succeeded())
		LightReceiver->SetStaticMesh(Octahedron.Object);
	LightReceiver->SetCastShadow(false);
	LightReceiver->bCastDynamicShadow = false;
	LightReceiver->SetRelativeScale3D(FVector(0.2f));
	LightDetectorLevel = 0.f;

	//Detector = CreateDefaultSubobject<UChildActorComponent>(TEXT("Detector"));


	// Create LightCameras
	//LightCamTop = CreateDefaultSubobject<UCameraComponent>(TEXT("LightCamTop"));
	//LightCamBottom = CreateDefaultSubobject<UCameraComponent>(TEXT("LightCamBottom"));
	//LightCamTop->SetupAttachment(LightReceiver);
	//LightCamBottom->SetupAttachment(LightReceiver);
	//LightCamTop->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	//LightCamTop->SetRelativeLocation(FVector(0.f, 0.f, 300.f));
	//LightCamTop->SetFieldOfView(66.f);
	//LightCamTop->SetAspectRatio(1.f);
	//LightCamTop->SetProjectionMode(ECameraProjectionMode::Perspective);
	//LightCamBottom->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	//LightCamBottom->SetRelativeLocation(FVector(0.f, 0.f, -300));
	//LightCamBottom->SetFieldOfView(66.f);
	//LightCamBottom->SetAspectRatio(1.f);
	//LightCamBottom->SetProjectionMode(ECameraProjectionMode::Perspective);

	//SceneCaptureTop = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureTop"));
	//SceneCaptureBottom = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureBottom"));
	//SceneCaptureTop->SetupAttachment(LightCamTop);
	//SceneCaptureBottom->SetupAttachment(LightCamBottom);
	//SceneCaptureTop->ProjectionType = ECameraProjectionMode::Perspective;
	//SceneCaptureBottom->ProjectionType = ECameraProjectionMode::Perspective;
	//SceneCaptureTop->FOVAngle = 36.f;
	//SceneCaptureBottom->FOVAngle = 36.f;	

	//static ConstructorHelpers::FObjectFinder<UTextureRenderTarget2D> RendTop(TEXT("TextureRenderTarget2D'/Game/Anachronia/IndividualPlaygrounds/Eddie/TestTextureRenderTarget2D_TOP.TestTextureRenderTarget2D_TOP'"));
	//static ConstructorHelpers::FObjectFinder<UTextureRenderTarget2D> RendBottom(TEXT("TextureRenderTarget2D'/Game/Anachronia/IndividualPlaygrounds/Eddie/TestTextureRenderTarget2D_BOTTOM.TestTextureRenderTarget2D_BOTTOM'"));

	//RenderTargetTop = RendTop.Object;
	//RenderTargetBottom = RendBottom.Object;
	//SceneCaptureTop->TextureTarget = RenderTargetTop;
	//SceneCaptureBottom->TextureTarget = RenderTargetBottom;

	// Initiate the stealth attributes
	PlayerVisibility = 0.f;
	PlayerLuminance = 0.f;
	PlayerNoiseLevel = 0.f;
	PlayerMotionLevel = 0.f;

	// Initiate base attributes
	PlayerSpeed = 600.f;
	PlayerCrouchedSpeed = 300.f;
	GetCharacterMovement()->JumpZVelocity = JumpVelocity;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetCharacterMovement()->MaxWalkSpeed = PlayerSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = PlayerCrouchedSpeed;
	CrouchedEyeHeight = 20.f;
	bPlayerIsCrouched = false;
	bPlayerIsSprinting = false;
	PlayerCapsuleStandingHalfHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	PlayerCapsuleCrouchedHalfHeight = 55.f;
	RelativeCamLocation = FirstPersonCameraComponent->GetRelativeLocation();
	RelativeCamCrouchedLocation = FVector(-39.56f, 1.75f, 34.f);


	// Initiate Main attributes
	MaxHealth = 100.f;
	SetCurrentHealth(GetMaxHealth());
	BaseDamage = 10.f;
	SetLethalDamage(BaseDamage, 0.f);	// The second argument should call the equipped weapon/tool fuction
	SetBluntDamage(BaseDamage, 0.f);	// When these functions are called outside the constructor

}

// Called when the game starts or when spawned
void AAnachroniaPlayer::BeginPlay()
{
	Super::BeginPlay();
	InitiatedWalkingSpeed = GetCharacterMovement()->MaxWalkSpeed;
}

// Called every frame
void AAnachroniaPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//UE_LOG(LogTemp, Warning, TEXT("Timer: %f"), DeltaTimeTimer);

	//SetGlobalLuminanceOnPlayer(FVector L);
	//PlayerLuminance = CalculateLuminance(GlobalLuminanceOnPlayer);

	SetLuminance(LightDetectorLevel);
	SetVisibility(PlayerLuminance, PlayerMotionLevel, VisibilityMinValue);
	LightReceiver->SetWorldRotation(FRotator(0.f, 0.f, 0.f));
	
	//SceneCaptureTop->CaptureScene();
	//SceneCaptureBottom->CaptureScene();	

	//if (DeltaTimeTimer > 1.f) {
	//	CheckLights();
	//	DeltaTimeTimer = 0.f;
	//}
	//DeltaTimeTimer += DeltaTime;
}

// Called to bind functionality to input
void AAnachroniaPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);



	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Sprint
	//PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AAnachroniaPlayer::Sprint);
	//PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AAnachroniaPlayer::UnSprint);


	//PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AAnachroniaPlayer::ToggleCrouch);



	// Bind movement events
	if (!bCannotMove) {
		PlayerInputComponent->BindAxis("MoveForward", this, &AAnachroniaPlayer::MoveForward);
		PlayerInputComponent->BindAxis("MoveRight", this, &AAnachroniaPlayer::MoveRight);
	}

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AAnachroniaPlayer::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AAnachroniaPlayer::LookUpAtRate);
}


void AAnachroniaPlayer::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AAnachroniaPlayer::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AAnachroniaPlayer::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AAnachroniaPlayer::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AAnachroniaPlayer::Sprint() {

	if (!bIsCrouched)
		GetCharacterMovement()->MaxWalkSpeed = InitiatedWalkingSpeed * 2;
	UE_LOG(LogTemp, Warning, TEXT("Sprinting!"))
}

void AAnachroniaPlayer::UnSprint() {
	GetCharacterMovement()->MaxWalkSpeed = InitiatedWalkingSpeed;
	UE_LOG(LogTemp, Warning, TEXT("Not Sprinting!"))
}

void AAnachroniaPlayer::ToggleCrouch() {
	if (!GetCharacterMovement()->IsCrouching())
		Crouch();
	else
		UnCrouch();

	//if (GetCharacterMovement()->IsCrouching())
	//	/*AAnachroniaPlayer::ToggleCrouchOff();*/
	//else {
	//	/*AAnachroniaPlayer::ToggleCrouchOn();*/
	//	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	//}
}

void  AAnachroniaPlayer::ToggleCrouchOn() {
	if (GetCharacterMovement())
	{
		if (CanCrouch())
		{
			GetCharacterMovement()->bWantsToCrouch = true;
		}
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		else if (!GetCharacterMovement()->CanEverCrouch())
		{
			UE_LOG(LogTemp, Log, TEXT("%s is trying to crouch, but crouching is disabled on this character! (check CharacterMovement NavAgentSettings)"), *GetName());
		}
#endif
	}
}

void  AAnachroniaPlayer::ToggleCrouchOff() {
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->bWantsToCrouch = false;
	}
}

bool AAnachroniaPlayer::CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation,
	int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor) const
{
	const float HalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const float ZOffsetList[]{HalfHeight, 0.0f, -HalfHeight};

	for (float ZOffset : ZOffsetList)
	{
		const FVector TargetLocation = GetActorLocation() + FVector(0.0f, 0.0f, ZOffset);
		
		FHitResult HitResult;
		const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, ObserverLocation, TargetLocation,
			ECC_Visibility,
			FCollisionQueryParams(SCENE_QUERY_STAT(AILineOfSight), true, IgnoreActor));

		NumberOfLoSChecksPerformed++;

		if (!bHit || (HitResult.Actor.IsValid() && HitResult.Actor->IsOwnedBy(this)))
		{
			// If this trace was blacked by nothing, or blocked by player itself (somehow), we can be seen
			OutSeenLocation = TargetLocation;
			OutSightStrength = 1.0f; // Maybe it would make sense to set this to visibility? But I'm not sure what it actually does...
			return true;
		}
	}

	OutSightStrength = 0.0f;
	return false;
}

float AAnachroniaPlayer::CalculateLuminance(const FVector& V) {

	/* NOTE: Check if the input value of the RGB vector is normalized (0.0-1.0) or not. if it's not,
			 then divide the values with 255 and then do the Sqrt-calculation */
	float R = V.X, G = V.Y, B = V.Z;
	float L = 0.f;
	L = FMath::Sqrt(FMath::Pow(0.299 * R, 2) + FMath::Pow(0.587 * G, 2) + FMath::Pow(0.114 * B, 2));
	return L;
}

//void AAnachroniaPlayer::CheckLights() {
//	//TArray <FLinearColor> LinearColorArrayTop;
//	SceneCaptureTop->CaptureScene();
//	TArray <FColor> ColorArrayTop;	
//	FRenderTarget* RenderTop = RenderTargetTop->GameThread_GetRenderTargetResource();
//	RenderTop->ReadPixels(ColorArrayTop, FReadSurfaceDataFlags(RCM_UNorm,CubeFace_MAX), FIntRect(32, 32, 200, 200));
//	RenderTargetTop->UpdateResourceImmediate();
//	// Index Formula: FColor PixelColorTop = ColorArrayTop[x + y * RenderTargetTop->SizeX]
//	
//	//FColor PixelColorTop = ColorArrayTop[0];
//	
//	//ColorArrayTop.Sort();
//	FColor PixelColorTop = ColorArrayTop[400]; //FindBrightestPixel(ColorArrayTop, 0, ColorArrayTop.Num());
//	UE_LOG(LogTemp, Warning, TEXT("Brightest Color: %f, %f, %f"), PixelColorTop.R, PixelColorTop.G, PixelColorTop.B);
//	
//}
//
//FColor& AAnachroniaPlayer::FindBrightestPixel(TArray<FColor>& ColorBuffer, int32 start, int32 max) {
//	FColor *BrightestColor = &ColorBuffer[100];
//	return *BrightestColor;
//}



void AAnachroniaPlayer::SaveGame() {
	UAnachroniaSaveGame* SaveGameInstance = Cast<UAnachroniaSaveGame>(UGameplayStatics::CreateSaveGameObject(UAnachroniaSaveGame::StaticClass()));

	//if (SaveGameInstance->CharacterStats.AchievementName.Num() <= 0)
	//	InitiateAchievements(SaveGameInstance);

	SaveGameInstance->CharacterStats.Health = CurrentHealth;
	SaveGameInstance->CharacterStats.MaxHealth = MaxHealth;
	SaveGameInstance->CharacterStats.ImperialMarks = ImperialMarks;
	SaveGameInstance->CharacterStats.Score = PlayerScore;
	//SaveGameInstance->CharacterStats.ReadBooksNames = PlayerReadBooksNames;

	/*SaveAchievementsStatus(SaveGameInstance);*/

	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex);
}
void AAnachroniaPlayer::LoadGame() {
	UAnachroniaSaveGame* LoadGameInstance = Cast<UAnachroniaSaveGame>(UGameplayStatics::CreateSaveGameObject(UAnachroniaSaveGame::StaticClass()));

	LoadGameInstance = Cast<UAnachroniaSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->SaveSlotName, LoadGameInstance->UserIndex));

	/*LoadAchievementsStatus(LoadGameInstance);*/

	CurrentHealth = LoadGameInstance->CharacterStats.Health;
	MaxHealth = LoadGameInstance->CharacterStats.MaxHealth;
	ImperialMarks = LoadGameInstance->CharacterStats.ImperialMarks;
	PlayerScore = LoadGameInstance->CharacterStats.Score;
	//PlayerReadBooksNames = LoadGameInstance->CharacterStats.ReadBooksNames;
}

//void AAnachroniaPlayer::ActivateAchievement(FName Name) {
//	if (PlayerAchievementsMap.Contains(Name)) {
//		PlayerAchievementsMap.Find(Name)->GetDefaultObject()->bIsAchieved = true;
//	}
//}
//
//
//void AAnachroniaPlayer::DisableAchievement(FName Name) {
//	if (PlayerAchievementsMap.Contains(Name)) {
//		PlayerAchievementsMap.Find(Name)->GetDefaultObject()->bIsAchieved = false;
//	}	
//}
//
//void AAnachroniaPlayer::SaveAchievementsStatus(UAnachroniaSaveGame* SaveGameInstance) {
//	for (auto& AchievementElement : PlayerAchievementsMap) {
//
//		for (int32 i = 0; i < SaveGameInstance->CharacterStats.AchievementName.Num(); i++) {
//			FName AchievementName = SaveGameInstance->CharacterStats.AchievementName[i];
//			if (AchievementName == AchievementElement.Key) {
//				UAchievement* PlayerAchievementObject = AchievementElement.Value.GetDefaultObject();
//				SaveGameInstance->CharacterStats.AchievementsAreActivated[i] = PlayerAchievementObject->bIsAchieved;
//			}
//		}
//	}
//}
//
//void AAnachroniaPlayer::LoadAchievementsStatus(UAnachroniaSaveGame* LoadGameInstance) {
//	for (int32 i = 0; i < LoadGameInstance->CharacterStats.AchievementName.Num(); i++) {
//		FName AchievementName = LoadGameInstance->CharacterStats.AchievementName[i];
//		for (auto& AchievementElement : PlayerAchievementsMap) {
//			if (AchievementName == AchievementElement.Key) {
//				AchievementElement.Value.GetDefaultObject()->bIsAchieved = LoadGameInstance->CharacterStats.AchievementsAreActivated[i];
//			}
//		}
//	}
//}
//
//void AAnachroniaPlayer::InitiateAchievements(UAnachroniaSaveGame* SaveGameInstance) {	
//	for (auto& AchievementElement : PlayerAchievementsMap) {
//		SaveGameInstance->CharacterStats.AchievementName.Add(AchievementElement.Value.GetDefaultObject()->Name);
//		SaveGameInstance->CharacterStats.AchievementsAreActivated.Add(AchievementElement.Value.GetDefaultObject()->bIsAchieved);
//	}	
//}
