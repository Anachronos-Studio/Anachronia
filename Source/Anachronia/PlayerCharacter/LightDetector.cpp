// Fill out your copyright notice in the Description page of Project Settings.


#include "LightDetector.h"
#include "Engine/TextureRenderTarget2D.h"
#include "UnrealClient.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ALightDetector::ALightDetector()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UTextureRenderTarget2D> RendTop(TEXT("TextureRenderTarget2D'/Game/Anachronia/IndividualPlaygrounds/Eddie/TestTextureRenderTarget2D_TOP.TestTextureRenderTarget2D_TOP'"));
	static ConstructorHelpers::FObjectFinder<UTextureRenderTarget2D> RendBottom(TEXT("TextureRenderTarget2D'/Game/Anachronia/IndividualPlaygrounds/Eddie/TestTextureRenderTarget2D_BOTTOM.TestTextureRenderTarget2D_BOTTOM'"));

	detectorTextureTop = RendTop.Object;
	detectorTextureBottom = RendBottom.Object;
}

// Called when the game starts or when spawned
void ALightDetector::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALightDetector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALightDetector::ProcessRenderTexture(UTextureRenderTarget2D* detectorTexture) {
	// Read the pixels from our RenderTexture and store the data into our color array
	// Note: ReadPixels is allegedly a very slow operation
	fRenderTarget = detectorTexture->GameThread_GetRenderTargetResource();
	fRenderTarget->ReadPixels(pixelStorage);

	// We iterate through every pixel we retrieved and find the brightest pixel
	for (int pixelNum = 0; pixelNum < pixelStorage.Num(); pixelNum++) {
		pixelChannelR = pixelStorage[pixelNum].R;
		pixelChannelG = pixelStorage[pixelNum].G;
		pixelChannelB = pixelStorage[pixelNum].B;

		// Use a formula to determine brightness based on pixel color values. Source for Formula used:
		// www.stackoverflow.com/questions/596216/formula-to-determine-brightness-of-rgb-color
		currentPixelBrightness = ((0.299 * pixelChannelR) + (0.587 * pixelChannelG) + (0.114 * pixelChannelB));

		// If the current pixel we just processed is brighter than the previously brightest pixel, replace it with the new pixel
		if (currentPixelBrightness >= brightnessOutput) {
			brightnessOutput = currentPixelBrightness;
		}
	}
}


float ALightDetector::CalculateBrightness() {
	// Ensure that the user has actually supplied us with RenderTextures
	if (detectorTextureTop == nullptr || detectorTextureBottom == nullptr) {
		return 0.0f;
	}
	// Reset our values for the next brightness test
	currentPixelBrightness = 0;
	brightnessOutput = 0;

	// Process our top and bottom RenderTextures
	ProcessRenderTexture(detectorTextureTop);
	ProcessRenderTexture(detectorTextureBottom);


	// At the end we return the brightest pixel we found in the RenderTextures
	brightnessOutput = UKismetMathLibrary::NormalizeToRange(brightnessOutput, 0.f, 255.f);
	return brightnessOutput;
}

float ALightDetector::CalculateGlobalCoefficient(float InputValue) {
	float GlobalCoefficient = UKismetMathLibrary::NormalizeToRange(InputValue, GlobalIlluminationMinValue, 1.0f);
	GlobalCoefficient = FMath::Clamp(GlobalCoefficient, 0.f, 1.f);
	return GlobalCoefficient;

	
}
