// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Achievement.generated.h"

/**
 * 
 */
UCLASS()
class ANACHRONIA_API UAchievement : public UUserWidget
{
	GENERATED_BODY()
	
public:

	
	/** Set Achievement name here */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Scoring & Achievements")
	FName Name;

	/** Set the description to the achievement*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Scoring & Achievements")
	FString Description;

	/** Checkbox wether the Achievement is achieved or not*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Scoring & Achievements")
	bool bIsAchieved;

	/** Set the image for the Achievement */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Scoring & Achievements")
	class UTexture* Image = CreateDefaultSubobject<UTexture>(TEXT("Image"));

	UFUNCTION(BlueprintCallable)
	FName GetAchievementName() { return Name; }
};
