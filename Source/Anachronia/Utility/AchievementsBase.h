// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBlueprint.h"
#include "AchievementsBase.generated.h"


/**
 * 
 */
UCLASS(Blueprintable)
class ANACHRONIA_API UAchievementsBase : public UWidgetBlueprint
{
	GENERATED_BODY()
	
public:
	UAchievementsBase();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Info)
	FName Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Info)
	FString Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Info)
	bool bIsAchieved;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Info)
	class UTexture* Image;
};
