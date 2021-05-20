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
	/** Unique identifier for this achievement. Don't change this after picking an initial name */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Scoring & Achievements")
	FName IdName;
	
	/** Set Achievement name shown to players here */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Scoring & Achievements")
	FString DisplayName;

	/** Set the description to the achievement*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Scoring & Achievements")
	FString Description;

	/** Checkbox wether the Achievement is achieved or not*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoring & Achievements")
	bool bIsAchieved;

	/** Set the image for the Achievement */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Scoring & Achievements")
	class UTexture* Image = CreateDefaultSubobject<UTexture>(TEXT("Image"));

	/** Array of names of goals */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Scoring & Achievements")
	TArray<FString> GoalNames;

	/** Array of goal booleans */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Scoring & Achievements")
	TArray<bool> GoalBools;

	/** Checks if it has multiple goals*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Scoring & Achievements")
	bool MultipleGoals;

	UFUNCTION(BlueprintCallable)
	FString GetAchievementName() { return DisplayName; }

	UFUNCTION(BlueprintCallable)
	void LoadFromSaveGame();
};
