// Fill out your copyright notice in the Description page of Project Settings.


#include "EscapeGameState.h"
#include "..\..\Public\Game\EscapeGameState.h"

AEscapeGameState::AEscapeGameState()
{
	RoundNumber = 1;
	ZombiesOnMap = 0;
}

uint16 AEscapeGameState::GetRoundNumber()
{
	return RoundNumber;
}

void AEscapeGameState::IncrementRoundNumber()
{
	++RoundNumber;
}
