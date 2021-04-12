// Fill out your copyright notice in the Description page of Project Settings.


#include "GuardPatrolPath.h"

#include "DrawDebugHelpers.h"
#include "Components/SplineComponent.h"

// Sets default values
AGuardPatrolPath::AGuardPatrolPath()
{
	// Tick is only used to draw editor visualizers
#if WITH_EDITOR
	PrimaryActorTick.bCanEverTick = true;
#endif // WITH_EDITOR


	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	RootComponent = SplineComponent;
	SplineComponent->SetClosedLoop(true);

}

// Called when the game starts or when spawned
void AGuardPatrolPath::BeginPlay()
{
	Super::BeginPlay();

	if (GetActorScale3D() != FVector::OneVector || RootComponent->GetRelativeScale3D() != FVector::OneVector)
	{
		GEngine->AddOnScreenDebugMessage(-1, 7.0f, FColor::Red,
			FString::Printf(TEXT("Guard patrol path (%s) has been scaled! This does not work well at the moment."), *GetName()));
	}
}

// Called every frame
void AGuardPatrolPath::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetWorld() && SplineComponent)
	{
		for (FPatrolStop& Stop : PatrolStops)
		{
			const float Length = 100.0f;
			FVector LineStart = SplineComponent->GetLocationAtSplineInputKey(Stop.InputKey, ESplineCoordinateSpace::World);
			FVector LineEnd = LineStart + FVector(0.0f, 0.0, Length);
			FVector Tangent = SplineComponent->GetTangentAtSplineInputKey(Stop.InputKey, ESplineCoordinateSpace::World);
			Tangent.Normalize();
			Tangent = FVector::CrossProduct(Tangent, FVector::UpVector); // We want the bitangent, actually
			DrawDebugLine(GetWorld(), LineStart, LineEnd, FColor::Yellow);
			DrawDebugLine(GetWorld(), LineStart - Tangent * 0.5f * Length, LineStart + Tangent * 0.5f * Length, FColor::Yellow);
		}
	}
}

void AGuardPatrolPath::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (SplineComponent != nullptr)
	{
		float LastInputKey = SplineComponent->GetNumberOfSplinePoints();
		if (!SplineComponent->IsClosedLoop())
		{
			LastInputKey = LastInputKey - 1.0f;
		}
		for (FPatrolStop& Stop : PatrolStops)
		{
			Stop.InputKey = FMath::Clamp(Stop.InputKey, 0.0f, LastInputKey);
		}
	}

}

bool AGuardPatrolPath::ShouldTickIfViewportsOnly() const
{
	return true;
}

// This function is include in a more recent Unreal version (4.26 I think), I retrieved it from GitHub
// https://github.com/EpicGames/UnrealEngine/commit/ebe07f17a101f2239d19016c1a3ce0a69a9dee52
float AGuardPatrolPath::GetDistanceAlongSplineAtSplineInputKey(float InKey) const
{
	check(SplineComponent != nullptr);

	const FSplineCurves& SplineCurves = SplineComponent->SplineCurves;
	const int32 NumPoints = SplineCurves.Position.Points.Num();
	const int32 NumSegments = SplineComponent->IsClosedLoop() ? NumPoints : NumPoints - 1;

	if ((InKey >= 0) && (InKey < NumSegments))
	{
		const int32 PointIndex = FMath::FloorToInt(InKey);
		const float Fraction = InKey - PointIndex;
		const int32 ReparamPointIndex = PointIndex * SplineComponent->ReparamStepsPerSegment;
		const float Distance = SplineCurves.ReparamTable.Points[ReparamPointIndex].InVal;
		// USplineComponent::GetSegmentLength() is private, so need to manually inline it below
		const float SegmentLength = SplineCurves.GetSegmentLength(PointIndex, Fraction, SplineComponent->IsClosedLoop(), SplineComponent->GetComponentTransform().GetScale3D());
		return Distance + SegmentLength;
	}
	else if (InKey >= NumSegments)
	{
		return SplineComponent->SplineCurves.GetSplineLength();
	}

	return 0.0f;
}

