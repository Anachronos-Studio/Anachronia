// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEquipItem.h"

// Sets default values
ABaseEquipItem::ABaseEquipItem()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	bIsWeapon = false;

	RootPlaceholder = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootPlaceholder->SetupAttachment(GetRootComponent());

	//SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	//SkeletalMesh->SetupAttachment(RootPlaceholder);
}

// Called when the game starts or when spawned
void ABaseEquipItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseEquipItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseEquipItem::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABaseEquipItem::Action() {
	
	if (bIsWeapon) {
		// TODO
		
	} else {
		// TODO
	}
}

void ABaseEquipItem::ChargeWeapon() {

}

void ABaseEquipItem::UseTool() {

}




