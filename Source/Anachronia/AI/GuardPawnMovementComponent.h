#pragma once

#include "CoreMinimal.h"

#include "GameFramework/PawnMovementComponent.h"
#include "GuardPawnMovementComponent.generated.h"

class GuardAIController;

/**
 * 
 */
UCLASS()
class ANACHRONIA_API UGuardPawnMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

public:
	UGuardPawnMovementComponent();

	UPROPERTY(Category = Guard, EditAnywhere, BlueprintReadWrite)
	float WalkSpeed = 200.0f;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual float GetMaxSpeed() const override;
	void MoveToLocation(FVector NewLocation, float DeltaTime);
};
