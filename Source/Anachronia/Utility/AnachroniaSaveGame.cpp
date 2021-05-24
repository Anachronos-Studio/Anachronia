// Fill out your copyright notice in the Description page of Project Settings.

#include "AnachroniaSaveGame.h"
#include "../Anachronia.h"
#include "../EquippableItems/BaseEquipItem.h"
#include "../Utility/Achievement.h"

UAnachroniaSaveGame::UAnachroniaSaveGame() {
	SaveSlotName = TEXT("Lucie");
	UserIndex = 0;
}

void UAnachroniaSaveGame::AchieveAchievement(FName AchievementName)
{
	UAnachroniaSaveGame* SaveGameInstance = Cast<UAnachroniaSaveGame>(UGameplayStatics::CreateSaveGameObject(UAnachroniaSaveGame::StaticClass()));
	FAchievementData Data = SaveGameInstance->Achievements.FindOrAdd(AchievementName);
	Data.bAchieved = true;
	SaveGameInstance->Achievements[AchievementName] = Data;
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex);
}

void UAnachroniaSaveGame::AchieveSubGoal(FName AchievementName, int GoalIndex)
{
	UAnachroniaSaveGame* SaveGameInstance = Cast<UAnachroniaSaveGame>(UGameplayStatics::CreateSaveGameObject(UAnachroniaSaveGame::StaticClass()));
	SaveGameInstance = Cast<UAnachroniaSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex));
	
	FAchievementData Data = SaveGameInstance->Achievements.FindOrAdd(AchievementName);
	Data.SubGoalsAchieved.SetNumZeroed(GoalIndex + 1, false);
	Data.SubGoalsAchieved[GoalIndex] = true;
	SaveGameInstance->Achievements[AchievementName] = Data;
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex);
}

bool UAnachroniaSaveGame::IsAchievementAchieved(FName IdName)
{
	UAnachroniaSaveGame* LoadGameInstance = Cast<UAnachroniaSaveGame>(UGameplayStatics::CreateSaveGameObject(UAnachroniaSaveGame::StaticClass()));

	LoadGameInstance = Cast<UAnachroniaSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->SaveSlotName, LoadGameInstance->UserIndex));

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

void UAnachroniaSaveGame::AddEquipItem(FName Name) {
	UAnachroniaSaveGame* SaveGameInstance = Cast<UAnachroniaSaveGame>(UGameplayStatics::CreateSaveGameObject(UAnachroniaSaveGame::StaticClass()));
	SaveGameInstance->CharacterItems.EquippableItems.AddUnique(Name);
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex);
}

void UAnachroniaSaveGame::AddReadBooks(FName Name) {
	UAnachroniaSaveGame* SaveGameInstance = Cast<UAnachroniaSaveGame>(UGameplayStatics::CreateSaveGameObject(UAnachroniaSaveGame::StaticClass()));
	SaveGameInstance->CharacterItems.ReadBooksNames.AddUnique(Name);
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex);
}
