// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomVolume.h"

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
		return FreePaths[0];
	}
}

void ARoomVolume::BeginPlay()
{
	for (AGuardPatrolPath* Path : PatrolPaths)
	{
		FreePaths.Add(Path);
	}
}
