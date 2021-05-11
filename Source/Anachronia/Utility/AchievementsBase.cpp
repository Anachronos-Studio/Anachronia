// Fill out your copyright notice in the Description page of Project Settings.


#include "AchievementsBase.h"
#include "Components/Image.h"

UAchievementsBase::UAchievementsBase() {

	Image = CreateDefaultSubobject<UTexture>(TEXT("Image"));
}