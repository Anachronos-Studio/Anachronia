// Copyright Epic Games, Inc. All Rights Reserved.

#include "AnachroniaGameMode.h"
#include "AnachroniaHUD.h"
#include "AnachroniaCharacter.h"
#include "FMODStudioModule.h"
#include "UObject/ConstructorHelpers.h"

AAnachroniaGameMode::AAnachroniaGameMode()
	: Super()
{
	// Force FMOD to initialize before loading player pawn.
	// Otherwise FMOD events referenced by Anachronia Player will just silently fail to load
	// See: https://qa.fmod.com/t/cannot-play-fmod-events-in-package-build/12248/3
	IFMODStudioModule::Get();
	
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/AnachroniaPlayerBP"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AAnachroniaHUD::StaticClass();
}
