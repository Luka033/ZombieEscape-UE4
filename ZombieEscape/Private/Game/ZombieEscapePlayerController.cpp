// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieEscapePlayerController.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"

void AZombieEscapePlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AZombieEscapePlayerController::GameHasEnded(class AActor* EndGameFocus, bool bIsWinner)
{
	Super::GameHasEnded(EndGameFocus, bIsWinner);
	if (bIsWinner)
	{
		UUserWidget* WinScreen = CreateWidget(this, WinScreenClass);
		if (WinScreen != nullptr)
		{
			WinScreen->AddToViewport();
		}
	}
	else
	{
		UUserWidget* LoseScreen = CreateWidget(this, LoseScreenClass);
		if (LoseScreen != nullptr)
		{
			LoseScreen->AddToViewport();
		}
	}
	GetWorldTimerManager().SetTimer(EndGameScreenTimer, this, &AZombieEscapePlayerController::SendToMainMenu, EndGameScreenDelay);
}

void AZombieEscapePlayerController::SendToMainMenu()
{
	GetWorld()->ServerTravel("/Game/BlueprintClasses/Game/MainMenu/Maps/MainMenuMap");
}
