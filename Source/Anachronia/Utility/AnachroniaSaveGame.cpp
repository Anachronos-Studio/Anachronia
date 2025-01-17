// Fill out your copyright notice in the Description page of Project Settings.

#include "AnachroniaSaveGame.h"
#include "../Anachronia.h"
#include "../Utility/Achievement.h"

UAnachroniaSaveGame::UAnachroniaSaveGame()
{
	SaveSlotName = TEXT("Lucie");
	UserIndex = 0;
}

void UAnachroniaSaveGame::AchieveAchievement(FName AchievementName)
{
	UAnachroniaSaveGame* SaveGameInstance = LoadSaveGame();
	FAchievementData Data = SaveGameInstance->Achievements.FindOrAdd(AchievementName);
	Data.bAchieved = true;
	SaveGameInstance->Achievements[AchievementName] = Data;
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex);
}

void UAnachroniaSaveGame::AchieveSubGoal(FName AchievementName, int GoalIndex)
{
	UAnachroniaSaveGame* SaveGameInstance = LoadSaveGame();
	
	FAchievementData Data = SaveGameInstance->Achievements.FindOrAdd(AchievementName);
	Data.SubGoalsAchieved.AddUnique(GoalIndex);
	SaveGameInstance->Achievements[AchievementName] = Data;
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex);
}

bool UAnachroniaSaveGame::IsAchievementAchieved(FName IdName)
{
	UAnachroniaSaveGame* LoadGameInstance = LoadSaveGame();

	TMap<FName, FAchievementData> AchievementsMap = LoadGameInstance->Achievements;

	if (AchievementsMap.Contains(IdName))
	{
		const FAchievementData Data = AchievementsMap[IdName];
		return Data.bAchieved;
	}
	else
	{
		return false;
	}
}

TArray<int32> UAnachroniaSaveGame::GetAchievedSubGoals(FName IdName)
{
	UAnachroniaSaveGame* LoadGameInstance = LoadSaveGame();

	TMap<FName, FAchievementData> AchievementsMap = LoadGameInstance->Achievements;

	if (AchievementsMap.Contains(IdName))
	{
		const FAchievementData Data = AchievementsMap[IdName];
		return Data.SubGoalsAchieved;
	}
	else
	{
		return TArray<int32>();
	}
}

void UAnachroniaSaveGame::AddEquipItem(FName Name)
{
	UAnachroniaSaveGame* SaveGameInstance = LoadSaveGame();
	SaveGameInstance->CharacterItems.EquippableItems.AddUnique(Name);
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex);
}

void UAnachroniaSaveGame::AddReadBooks(FName Name)
{
	UAnachroniaSaveGame* SaveGameInstance = LoadSaveGame();
	SaveGameInstance->CharacterItems.ReadBooksNames.AddUnique(Name);
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex);
}

TArray<FName> UAnachroniaSaveGame::GetUnlockedItems()
{
	UAnachroniaSaveGame* LoadGameInstance = LoadSaveGame();

	return LoadGameInstance->CharacterItems.EquippableItems;
}

UAnachroniaSaveGame* UAnachroniaSaveGame::LoadSaveGame()
{
	UAnachroniaSaveGame* SaveGameInstance = Cast<UAnachroniaSaveGame>(UGameplayStatics::CreateSaveGameObject(UAnachroniaSaveGame::StaticClass()));
	if (UGameplayStatics::DoesSaveGameExist(SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex))
	{
		SaveGameInstance = Cast<UAnachroniaSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex));
	}

	return SaveGameInstance;
}

TArray<FName> UAnachroniaSaveGame::GetReadBooks() {
	UAnachroniaSaveGame* SaveGameInstance = Cast<UAnachroniaSaveGame>(UGameplayStatics::CreateSaveGameObject(UAnachroniaSaveGame::StaticClass()));
	TArray<FName> ReturnValue;
	if (UGameplayStatics::DoesSaveGameExist(SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex)) {
		SaveGameInstance = Cast<UAnachroniaSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex));
		ReturnValue = SaveGameInstance->CharacterItems.ReadBooksNames;
	}
	return ReturnValue;
}
