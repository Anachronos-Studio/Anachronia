// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BaseEquipItem.generated.h"

class AAnachroniaPlayer;
UCLASS()
class ANACHRONIA_API ABaseEquipItem : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABaseEquipItem();

	UPROPERTY(VisibleAnywhere)
	class USceneComponent* RootPlaceholder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	class UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	class USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Player)
	AAnachroniaPlayer* AttachedPlayer;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


protected:
	// Checks if the equippable item is a weapon or not
	bool bIsWeapon;


public:

	bool IsItWeapon() { return bIsWeapon; }

	UFUNCTION(BlueprintImplementableEvent, Category = InputEvents)
	void CallPrimaryAction();
	UFUNCTION(BlueprintImplementableEvent, Category = InputEvents)
	void CallSecondaryAction();	

	UFUNCTION(BlueprintImplementableEvent, Category = Attachments)
	void AttachToPlayer();
};
