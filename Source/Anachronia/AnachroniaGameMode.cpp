// Copyright Epic Games, Inc. All Rights Reserved.

#include "AnachroniaGameMode.h"
#include "AnachroniaHUD.h"
#include "AnachroniaCharacter.h"
#include "UObject/ConstructorHelpers.h"

AAnachroniaGameMode::AAnachroniaGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/AnachroniaPlayerBP"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AAnachroniaHUD::StaticClass();
}
