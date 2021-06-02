// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "AnachroniaSaveGame.generated.h"


USTRUCT(BlueprintType)
struct FAchievementData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Achievements)
	bool bAchieved;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Achievements)
	TArray<int32> SubGoalsAchieved;
};

USTRUCT(BlueprintType)
struct FCharacterStats
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveGameData)
	int32 ImperialMarks;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveGameData)
	int32 Score;

	//TMap<FName, TSubclassOf<class ABaseEquipItem>> Items;

};

USTRUCT(BlueprintType)
struct FCharacterItems {
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveGameData)
	TArray<FName> ReadBooksNames;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveGameData)
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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Basic)
	FCharacterStats CharacterStats;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Basic)
	TMap<FName, FAchievementData> Achievements;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Basic)
	FCharacterItems CharacterItems;

	UFUNCTION(BlueprintCallable)
	static void AchieveAchievement(FName AchievementName);

	UFUNCTION(BlueprintCallable)
	static void AchieveSubGoal(FName AchievementName, int GoalIndex);

	UFUNCTION(BlueprintCallable)
	static bool IsAchievementAchieved(FName IdName);

	UFUNCTION(BlueprintCallable)
	static TArray<int32> GetAchievedSubGoals(FName IdName);

	UFUNCTION(BlueprintCallable)
	static void AddEquipItem(FName Name);

	UFUNCTION(BlueprintCallable)
	static void AddReadBooks(FName Name);

	UFUNCTION(BlueprintCallable)
	static TArray<FName> GetReadBooks();

	UFUNCTION(BlueprintCallable)
	static TArray<FName> GetUnlockedItems();

	UFUNCTION(BlueprintCallable)
	static UAnachroniaSaveGame* LoadSaveGame();
};
