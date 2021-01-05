// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "EscapeGameState.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIEESCAPE_API AEscapeGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	AEscapeGameState();

protected:
	uint16 RoundNumber;
	uint8 ZombiesOnMap;

public:
	uint16 GetRoundNumber();

	void IncrementRoundNumber();
	
};
