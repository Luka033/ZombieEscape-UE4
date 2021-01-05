// Fill out your copyright notice in the Description page of Project Settings.


#include "EscapeZombieGameInstanceBase.h"
#include "Engine/World.h"


UEscapeZombieGameInstanceBase::UEscapeZombieGameInstanceBase()
{

}

void UEscapeZombieGameInstanceBase::GoToMap(FString MAPURL)
{
	GetWorld()->ServerTravel(MAPURL);
}
