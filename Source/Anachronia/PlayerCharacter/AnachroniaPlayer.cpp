// Fill out your copyright notice in the Description page of Project Settings.


#include "AnachroniaPlayer.h"

#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AAnachroniaPlayer::AAnachroniaPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

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

	// Create a box that will catch the lighing
	LightReceiver = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LightReceiver"));
	LightReceiver->SetupAttachment(GetCapsuleComponent());
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Octahedron(TEXT("StaticMesh'/Game/Anachronia/Meshes/Oct.Oct'"));
	if (Octahedron.Succeeded()) 
		LightReceiver->SetStaticMesh(Octahedron.Object);
	LightReceiver->SetCastShadow(false);
	LightReceiver->bCastDynamicShadow = false;
	LightReceiver->SetRelativeScale3D(FVector(0.2f));

	// Initiate the stealth attributes
	PlayerVisibility = 0.f;
	PlayerLuminance = 0.f;
	PlayerNoiseLevel = 0.f;
	PlayerMotionLevel = 0.f;

	// Initiate base attributes
	GetCharacterMovement()->JumpZVelocity = JumpVelocity;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;

	CrouchedEyeHeight = 20.f;
}

// Called when the game starts or when spawned
void AAnachroniaPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	bIsCrouched = false;
}

// Called every frame
void AAnachroniaPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAnachroniaPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	//Bind crouch events
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AAnachroniaPlayer::ToggleCrouch);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AAnachroniaPlayer::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AAnachroniaPlayer::MoveRight);

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

float AAnachroniaPlayer::CalculateLuminance(FVector V) {

	/* NOTE: Check if the input value of the RGB vector is normalized (0.0-1.0) or not. if it's not, 
			 then divide the values with 255 and then do the Sqrt-calculation */
	float R = V.X, G = V.Y, B = V.Z;
	float L = 0.f;
	L = FMath::Sqrt(FMath::Pow(0.299 * R, 2) + FMath::Pow(0.587 * G, 2) + FMath::Pow(0.114 * B, 2));
	return L;
}

void AAnachroniaPlayer::SetLuminance(float Value) {

}

void AAnachroniaPlayer::SetMotionLevel(float Value) {

}

void AAnachroniaPlayer::SetVisibility(float L, float M) {

}

float AAnachroniaPlayer::GetLuminance() {
	return PlayerLuminance;
}