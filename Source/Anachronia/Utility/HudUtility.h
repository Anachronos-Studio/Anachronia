// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HudUtility.generated.h"

/**
 * 
 */
UCLASS()
class ANACHRONIA_API UHudUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	/**
	 * Converts a world location to screen position for HUD drawing. This differs from the results of FSceneView::WorldToScreen in that it returns a position along the edge of the screen for offscreen locations
	 *
	 * @param		Location	- The world space location to be converted to screen space
	 * @param		EdgePercent - How close to the edge of the screen, 1.0 = at edge, 0.0 = at center of screen. .9 or .95 is usually desirable
	 * @param		OutScreenPosition - the screen coordinates for HUD drawing
	 * @param		bOutIsOnScreen - True if the specified location is in the camera view (may be obstructed)
	 */
	
	UFUNCTION(BlueprintPure, meta = (WorldContext="WorldContextObject", CallableWithoutWorldContext), Category = "Widget")
	static void FindScreenEdgeLocationForWorldLocation(UObject* WorldContextObject, const FVector& InLocation, const FVector2D Margin, FVector2D& OutScreenPosition, float& OutRotationAngleDegrees, bool& bIsOnScreen);

};
