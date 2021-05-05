// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnachroniaEventSystem.generated.h"

USTRUCT(BlueprintType)
struct FAnachroniaNoiseInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Location;

	// Who made the noise?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* Instigator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxRange = 500.0f;

	// If the noise is heard, controls how much it affects the guard's SusValue
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SusMultiplier = 1.0f;

	// Can normally be left as None, used to tag some special kinds of sounds
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Tag = NAME_None;
};

UCLASS()
class ANACHRONIA_API UAnachroniaEventSystem : public UObject
{
	GENERATED_BODY()
	
public:
	static UAnachroniaEventSystem* GetInstance();
	
	// Sets default values for this actor's properties
	UAnachroniaEventSystem();
	DECLARE_MULTICAST_DELEGATE_OneParam(FAnachroniaNoiseEvent, FAnachroniaNoiseInfo)
	FAnachroniaNoiseEvent AnachroniaNoiseEvent;

	UFUNCTION(BlueprintCallable)
	static void ReportAnachroniaNoise(const FAnachroniaNoiseInfo NoiseInfo);
	
private:
	static UAnachroniaEventSystem* Instance;
};
