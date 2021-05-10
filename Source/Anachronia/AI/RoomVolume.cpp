// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomVolume.h"

#include "GuardPatrolPath.h"
#include "Components/BrushComponent.h"

ARoomVolume::ARoomVolume()
{
	GetBrushComponent()->SetCollisionProfileName(TEXT("OverlapGuardOnly"));
}

void ARoomVolume::ClaimPath(AGuardPatrolPath* Path)
{
	FreePaths.Remove(Path);
}

void ARoomVolume::AbandonPath(AGuardPatrolPath* Path)
{
	FreePaths.AddUnique(Path);
}

AGuardPatrolPath* ARoomVolume::FindFreePath()
{
	if (FreePaths.Num() == 0)
	{
		return nullptr;
	}
	else
	{
		return FreePaths.Last();
	}
}

void ARoomVolume::BeginPlay()
{
	for (AGuardPatrolPath* Path : PatrolPaths)
	{
		FreePaths.Add(Path);
		Path->AssignToRoom(this);
	}
}

#if WITH_EDITOR
void ARoomVolume::OnConstruction(const FTransform& Transform)
{
	for (AGuardPatrolPath* Path : PatrolPaths)
	{
		if (Path != nullptr)
		{
			if (Path->GetRoom() != nullptr && Path->GetRoom() != this)
			{
				Path->GetRoom()->PatrolPaths.Remove(Path);
			}
			Path->AssignToRoom(this);
		}
	}
}
#endif // WITH_EDITOR
