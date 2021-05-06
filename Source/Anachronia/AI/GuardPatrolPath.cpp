// Fill out your copyright notice in the Description page of Project Settings.


#include "GuardPatrolPath.h"

#include "RoomVolume.h"
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

void AGuardPatrolPath::AssignToRoom(ARoomVolume* InRoom)
{
	if (Room != nullptr)
	{
		const FString Msg = FString::Printf(TEXT("Trying to assign patrol path %s to multiple rooms (previously: %s, now: %s)"), *GetName(), *Room->GetName(), *InRoom->GetName());
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, Msg);
	}
	
	Room = InRoom;
}

void AGuardPatrolPath::Claim()
{
}

void AGuardPatrolPath::Abandon()
{
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
			FVector LineStart = SplineComponent->GetLocationAtSplinePoint(Stop.Where, ESplineCoordinateSpace::World);
			FVector LineEnd = LineStart + FVector(0.0f, 0.0, Length);
			FVector Tangent = SplineComponent->GetTangentAtSplinePoint(Stop.Where, ESplineCoordinateSpace::World);
			Tangent.Normalize();
			Tangent = FVector::CrossProduct(Tangent, FVector::UpVector); // We want the bitangent, actually
			DrawDebugLine(GetWorld(), LineStart, LineEnd, FColor::Yellow);
			DrawDebugLine(GetWorld(), LineStart - Tangent * 0.5f * Length, LineStart + Tangent * 0.5f * Length, FColor::Yellow);
		}
	}
}

void AGuardPatrolPath::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (SplineComponent != nullptr)
	{
		const int NumPoints = SplineComponent->GetNumberOfSplinePoints();
		for (FPatrolStop& Stop : PatrolStops)
		{
			Stop.Where = FMath::Clamp(Stop.Where, 0, NumPoints - 1);
		}
		
		for (int32 PointIndex = 0; PointIndex < SplineComponent->GetNumberOfSplinePoints(); PointIndex++)
		{
			SplineComponent->SetSplinePointType(PointIndex, ESplinePointType::Linear);
		}
	}
}

int32 AGuardPatrolPath::FindClosestPointToWorldLocation(FVector FromLocation) const
{
	check(SplineComponent != nullptr);

	float SmallestDistanceSoFar = BIG_NUMBER;
	int ClosestPoint = 0;
	for (int32 Point = 0; Point < SplineComponent->GetNumberOfSplinePoints(); Point++)
	{
		const FVector Location = SplineComponent->GetWorldLocationAtSplinePoint(Point);
		const float Distance = FVector::Dist2D(Location, FromLocation);
		if (Distance < SmallestDistanceSoFar)
		{
			SmallestDistanceSoFar = Distance;
			ClosestPoint = Point;
		}
	}

	return ClosestPoint;
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

