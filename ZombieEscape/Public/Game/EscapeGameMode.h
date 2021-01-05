 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EscapeGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIEESCAPE_API AEscapeGameMode : public AGameModeBase
{
	GENERATED_BODY()


public:
	AEscapeGameMode();

protected:
	class AEscapeGameState* EscapeGameState;
	virtual void BeginPlay() override;

	TArray<class AZombieEscapeSpawnPoint*> ZombieSpawnPoints;
	TArray<class AZombieEscapeSpawnPoint*> ActiveZombieSpawnPoints;

	TArray<uint8> ActiveZones;


	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
	TSubclassOf<class AZombieBase> ZombieClass;

	FTimerHandle TZombieSpawnHandle;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
	uint16 MaxZombiesOnMap;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
	float ZombieSpawnTimer;

	uint16 ZombiesRemaining;
	uint8 ZombiesOnMap;

protected:
	void CalculateZombieCount();
	void SpawnZombie();
	

public:
	void NewZoneActive(uint8 ZoneNumber);
	void DecrementZombiesOnMapCount();
	virtual void PawnKilled(APawn* PawnKilled);
	void IncrementMaxZombiesOnMap();


};
