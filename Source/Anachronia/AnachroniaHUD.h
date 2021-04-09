// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AnachroniaHUD.generated.h"

UCLASS()
class AAnachroniaHUD : public AHUD
{
	GENERATED_BODY()

public:
	AAnachroniaHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

