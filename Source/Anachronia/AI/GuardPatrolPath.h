// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GuardPatrolPath.generated.h"

class USplineComponent;

USTRUCT()
struct FPatrolStop
{
	GENERATED_BODY()

	// At which point on spline to stop
	UPROPERTY(EditAnywhere)
	int Where;

	// How long to stop for in seconds
	UPROPERTY(EditAnywhere)
	float Duration;
};

UCLASS()
class ANACHRONIA_API AGuardPatrolPath : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGuardPatrolPath();

	FORCEINLINE USplineComponent* GetSpline() const { return SplineComponent; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	USplineComponent* SplineComponent;

	
public:
	UPROPERTY(Category = "Spline|Patrol", EditAnywhere)
	TArray<FPatrolStop> PatrolStops;

	// Only used when spline is not a closed loop. If ping pong is true, guard will move back and forth. If false, they will just stop when reaching the end
	UPROPERTY(Category = "Spline|Patrol", EditAnywhere)
	bool bPingPong;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	int32 FindClosestPointToWorldLocation(FVector FromLocation) const;
	virtual bool ShouldTickIfViewportsOnly() const override;
	float GetDistanceAlongSplineAtSplineInputKey(float InKey) const;
};
