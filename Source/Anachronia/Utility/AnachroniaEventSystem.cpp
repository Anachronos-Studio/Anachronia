// Fill out your copyright notice in the Description page of Project Settings.


#include "AnachroniaEventSystem.h"

UAnachroniaEventSystem* UAnachroniaEventSystem::Instance = nullptr;

UAnachroniaEventSystem::UAnachroniaEventSystem()
{
	
}

UAnachroniaEventSystem* UAnachroniaEventSystem::GetInstance()
{
	if (Instance == nullptr)
	{
		Instance = NewObject<UAnachroniaEventSystem>();
		checkf(Instance != nullptr, TEXT("Failed to create AnachroniaEventSystem instance!"));
		Instance->AddToRoot();
	}

	return Instance;
}

void UAnachroniaEventSystem::ReportAnachroniaNoise(const FAnachroniaNoiseInfo NoiseInfo)
{
	GetInstance()->AnachroniaNoiseEvent.Broadcast(NoiseInfo);
}
