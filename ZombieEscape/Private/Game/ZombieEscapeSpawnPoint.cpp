// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieEscapeSpawnPoint.h"
#include "..\..\Public\Game\ZombieEscapeSpawnPoint.h"
#include "..\..\Usables\Door.h"

AZombieEscapeSpawnPoint::AZombieEscapeSpawnPoint()
{
	bIsUsed = false;
	Zone = 0;
	bIsActive = false;
}

void AZombieEscapeSpawnPoint::BeginPlay()
{
	Super::BeginPlay();

	
}

uint8 AZombieEscapeSpawnPoint::GetZone()
{
	return Zone;
}

void AZombieEscapeSpawnPoint::SetZone(uint8 ZoneNumber)
{
	Zone = ZoneNumber;
}

void AZombieEscapeSpawnPoint::Activate()
{
	bIsActive = true;
}

bool AZombieEscapeSpawnPoint::IsActive()
{
	return bIsActive;
}


bool AZombieEscapeSpawnPoint::IsUsed() {
	return bIsUsed;
}

void AZombieEscapeSpawnPoint::SetUsed(bool Used)
{
	bIsUsed = Used;
}

ADoor* AZombieEscapeSpawnPoint::GetLinkedBarricade()
{
	return LinkedBarricade;
}
