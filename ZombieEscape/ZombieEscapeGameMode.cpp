// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "ZombieEscapeGameMode.h"
#include "ZombieEscapeHUD.h"
#include "CharacterBase.h"
#include "UObject/ConstructorHelpers.h"

AZombieEscapeGameMode::AZombieEscapeGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/BlueprintClasses/Player/BP_CharacterBase.BP_CharacterBase_C"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AZombieEscapeHUD::StaticClass();
}
