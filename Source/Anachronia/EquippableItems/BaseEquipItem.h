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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SaveData)
    FName ItemName;

    UPROPERTY(VisibleAnywhere)
    class USceneComponent* RootPlaceholder;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
    class UStaticMeshComponent* StaticMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
    class USkeletalMeshComponent* SkeletalMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Player)
    AAnachroniaPlayer* AttachedPlayer;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
    float BaseWeaponDamage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
    float StealthDamageMultiplier;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Attributes)
    float OutputDamage;

    // Checks if the equippable item is a weapon or not
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
    bool bIsWeapon;

    /** Optional, only for ranged weapon. this value should be 0 for tools and melee weapons */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Ranged")
    float WeaponRange;

    /** Optional, for weapons that uses ammo... like all ranged weapons (pun intended) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Ranged")
    float WeaponAmmo;

    /** Optional, for weapons that requires charging of some kind.. usually a magic/energy based ranged weapon */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Ranged")
    float ChargeTime;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


protected:



public:

    bool IsItWeapon() { return bIsWeapon; }

    UFUNCTION(BlueprintImplementableEvent, Category = InputEvents)
        void CallPrimaryAction();
    UFUNCTION(BlueprintImplementableEvent, Category = InputEvents)
        void CallSecondaryAction();

    UFUNCTION(BlueprintImplementableEvent, Category = Attachments)
        void AttachToPlayer();

    /** For weapons that deal LEATHAL damage, implement the event/function on the BP subclass of BaseEquipItem */
    UFUNCTION(BlueprintImplementableEvent, Category = Attributes)
    void DealLethalDamage();

    /** For weapons that deal BLUNT damage, implement the event/function on the BP subclass of BaseEquipItem */
    UFUNCTION(BlueprintImplementableEvent, Category = Attributes)
    void DealBluntDamage();

    UFUNCTION(BlueprintCallable, Category = Attributes)
    void SetItemTag(FName& InputString) {
        Tags.Add(InputString);
    }
};