// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "AnachroniaSaveGame.generated.h"


USTRUCT(BlueprintType)
struct FAchievementData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = Achievements)
	bool bAchieved;

	UPROPERTY(VisibleAnywhere, Category = Achievements)
	TArray<bool> SubGoalsAchieved;
};

USTRUCT(BlueprintType)
struct FCharacterStats
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = SaveGameData)
	float Health;

	UPROPERTY(VisibleAnywhere, Category = SaveGameData)
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, Category = SaveGameData)
	int32 ImperialMarks;

	UPROPERTY(VisibleAnywhere, Category = SaveGameData)
	int32 Score;

	UPROPERTY(VisibleAnywhere, Category = SaveGameData)
	TArray<FName> ReadBooksNames;

	//TMap<FName, TSubclassOf<class ABaseEquipItem>> Items;

	UPROPERTY(VisibleAnywhere, Category = SaveGameData)
	TArray<FName> EquippableItems;
};

/**
 * 
 */
UCLASS()
class ANACHRONIA_API UAnachroniaSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:

	UAnachroniaSaveGame();

	UPROPERTY(VisibleAnywhere, Category = Basic)
	FString SaveSlotName;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	uint32 UserIndex;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	FCharacterStats CharacterStats;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	TMap<FName, FAchievementData> Achievements;

	UFUNCTION(BlueprintCallable)
	static void AchieveAchievement(FName AchievementName);

	UFUNCTION(BlueprintCallable)
	static void AchieveSubGoal(FName AchievementName, int GoalIndex);

	UFUNCTION(BlueprintCallable)
	static bool IsAchievementAchieved(FName IdName);
};
