// Fill out your copyright notice in the Description page of Project Settings.


#include "Achievement.h"

#include "AnachroniaSaveGame.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"


void UAchievement::LoadFromSaveGame()
{
	UAnachroniaSaveGame* LoadGameInstance = Cast<UAnachroniaSaveGame>(UGameplayStatics::CreateSaveGameObject(UAnachroniaSaveGame::StaticClass()));

	LoadGameInstance = Cast<UAnachroniaSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->SaveSlotName, LoadGameInstance->UserIndex));

	TMap<FName, FAchievementData> AchievementsMap = LoadGameInstance->Achievements;

	if (AchievementsMap.Contains(IdName))
	{
		const FAchievementData Data = AchievementsMap[IdName];
		bIsAchieved = Data.bAchieved;
		GoalsAchieved = Data.SubGoalsAchieved;
	}
	else
	{
		bIsAchieved = false;
		GoalsAchieved.Empty();
	}
}
