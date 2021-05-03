// Fill out your copyright notice in the Description page of Project Settings.


#include "HudUtility.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

/* Based on solution by Jeff LaMarche
 * https://github.com/jlamarche/UE4-Offscreen-Indicator | https://forums.unrealengine.com/t/easy-offscreen-indicator-blueprint-node/21342
 * I place no restrictions on the use of this. You can use it, modify it, redistribute it, or ignore it with no obligations. I hope it’s helpful to somebody.
 */

void UHudUtility::FindScreenEdgeLocationForWorldLocation(UObject* WorldContextObject, const FVector& InLocation, const FVector2D Margin, FVector2D& OutScreenPosition, float& OutRotationAngleDegrees, bool& bIsOnScreen)
{
		bIsOnScreen = false;
		OutRotationAngleDegrees = 0.f;
		FVector2D ScreenPosition;

		if (!GEngine)
		{
			return;
		}

		const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
		const FVector2D ViewportCenter = FVector2D(ViewportSize.X * 0.5f, ViewportSize.Y * 0.5f);

		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject);

		if (!World)
		{
			return;
		}

		APlayerController* PlayerController = (WorldContextObject ? UGameplayStatics::GetPlayerController(WorldContextObject, 0) : NULL);

		if (!PlayerController)
		{
			return;
		}

		FVector CameraLoc;
		FRotator CameraRot;

		PlayerController->GetPlayerViewPoint(CameraLoc, CameraRot);

		const FVector CameraToLoc = InLocation - CameraLoc;
		FVector Forward = CameraRot.Vector();
		FVector Offset = CameraToLoc.GetSafeNormal();

		float DotProduct = FVector::DotProduct(Forward, Offset);
		bool bLocationIsBehindCamera = (DotProduct < 0);

		if (bLocationIsBehindCamera)
		{
			FVector Inverted = CameraToLoc * -1.f;
			FVector NewInLocation = CameraLoc + Inverted;
			
			PlayerController->ProjectWorldLocationToScreen(NewInLocation, ScreenPosition);

			ScreenPosition.X = ViewportSize.X - ScreenPosition.X;
			//ScreenPosition.Y = ViewportSize.Y - ScreenPosition.Y;
		}
		else
		{
			PlayerController->ProjectWorldLocationToScreen(InLocation, ScreenPosition);
		}
	
		// Check to see if it's on screen. If it is, ProjectWorldLocationToScreen is all we need, return it.
		if (ScreenPosition.X >= 0.f && ScreenPosition.X <= ViewportSize.X
			&& ScreenPosition.Y >= 0.f && ScreenPosition.Y <= ViewportSize.Y && !bLocationIsBehindCamera)
		{
			ScreenPosition.X = FMath::Clamp(ScreenPosition.X, Margin.X, ViewportSize.X - Margin.X);
			ScreenPosition.Y = FMath::Clamp(ScreenPosition.Y, Margin.Y, ViewportSize.Y - Margin.Y);
			
			OutScreenPosition = ScreenPosition;
			bIsOnScreen = true;
			return;
		}

		ScreenPosition -= ViewportCenter;

		float AngleRadians = FMath::Atan2(ScreenPosition.Y, ScreenPosition.X);
		AngleRadians -= FMath::DegreesToRadians(90.f);

		OutRotationAngleDegrees = FMath::RadiansToDegrees(AngleRadians) + 180.f;

		float Cos = FMath::Cos(AngleRadians);
		float Sin = -FMath::Sin(AngleRadians);

		ScreenPosition = FVector2D(ViewportCenter.X + (Sin * 150.f), ViewportCenter.Y + Cos * 150.f);

		float m = Cos / Sin;

		const FVector2D ScreenBounds = FVector2D::Max(FVector2D::ZeroVector, ViewportCenter - Margin);

		if (Cos > 0)
		{
			ScreenPosition = FVector2D(ScreenBounds.Y / m, ScreenBounds.Y);
		}
		else
		{
			ScreenPosition = FVector2D(-ScreenBounds.Y / m, -ScreenBounds.Y);
		}

		if (ScreenPosition.X > ScreenBounds.X)
		{
			ScreenPosition = FVector2D(ScreenBounds.X, ScreenBounds.X * m);
		}
		else if (ScreenPosition.X < -ScreenBounds.X)
		{
			ScreenPosition = FVector2D(-ScreenBounds.X, -ScreenBounds.X * m);
		}

		ScreenPosition += ViewportCenter;

		OutScreenPosition = ScreenPosition;
}
