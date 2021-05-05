// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "AnachroniaSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FCharacterStats {
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = SaveGameData)
	float Health;

	UPROPERTY(VisibleAnywhere, Category = SaveGameData)
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, Category = SaveGameData)
	int32 ImperialMarks;

	UPROPERTY(VisibleAnywhere, Category = SaveGameData)
	int32 Score;
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
};
