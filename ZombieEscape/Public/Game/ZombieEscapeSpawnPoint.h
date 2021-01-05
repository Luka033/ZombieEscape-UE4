// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "ZombieEscapeSpawnPoint.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIEESCAPE_API AZombieEscapeSpawnPoint : public ATargetPoint
{
	GENERATED_BODY()

public:
	AZombieEscapeSpawnPoint();

protected:
	bool bIsUsed;

public:
	bool IsUsed();
	void SetUsed(bool Used);
	class ADoor* GetLinkedBarricade();

protected:
	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
	class ADoor* LinkedBarricade;

	uint8 Zone;
	bool bIsActive;

	virtual void BeginPlay() override;

public:
	uint8 GetZone();
	void SetZone(uint8 ZoneNumber);
	void Activate();
	bool IsActive();
};
