// Fill out your copyright notice in the Description page of Project Settings.


#include "EscapeGameMode.h"
#include "ZombieEscapeSpawnPoint.h"
#include "ZombieEscapeHUD.h"
#include "CharacterBase.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Containers/Array.h"
#include "..\..\Public\Game\EscapeGameMode.h"
#include "..\..\Usables\Door.h"
#include "TimerManager.h"
#include "ZombieBase.h"
#include "EscapeGameState.h"
#include "Engine/World.h"

AEscapeGameMode::AEscapeGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/BlueprintClasses/Player/BP_CharacterBase.BP_CharacterBase_C"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	ZombiesRemaining = 5;
	ZombiesOnMap = 0;
	ZombieSpawnTimer = 4.0f;
	ActiveZones = {};

	// use our custom HUD class
	HUDClass = AZombieEscapeHUD::StaticClass();
}

void AEscapeGameMode::BeginPlay()
{
	Super::BeginPlay();	

	FString MapName = GetWorld()->GetMapName();
	/*UE_LOG(LogTemp, Warning, TEXT("LEVEL NAME: %s"), *MapName);*/
	if (MapName.Contains("Dungeon")) {
		MaxZombiesOnMap = 10;
	}
	else
	{
		MaxZombiesOnMap = 8;
	}

	EscapeGameState = GetGameState<AEscapeGameState>();
	/*CalculateZombieCount();*/
	

	TArray<AActor*> TempActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AZombieEscapeSpawnPoint::StaticClass(), TempActors);
	for (AActor* Actor : TempActors)
	{
		if (AZombieEscapeSpawnPoint* SpawnPoint = Cast<AZombieEscapeSpawnPoint>(Actor))
		{
			if (ADoor* LinkedBarricade = SpawnPoint->GetLinkedBarricade())
			{
				SpawnPoint->SetZone(LinkedBarricade->GetAccessZone());
				ZombieSpawnPoints.Add(SpawnPoint);
				/*UE_LOG(LogTemp, Warning, TEXT("Zone Number: %d"), LinkedBarricade->GetAccessZone());*/
			}
			else 
			{
				ActiveZombieSpawnPoints.Add(SpawnPoint);
			}
		}
	}	
	GetWorld()->GetTimerManager().SetTimer(TZombieSpawnHandle, this, &AEscapeGameMode::SpawnZombie, ZombieSpawnTimer, true);
	
}

void AEscapeGameMode::CalculateZombieCount()
{
	if (EscapeGameState)
	{
		uint16 RoundNumber = EscapeGameState->GetRoundNumber();
		// do calculation here
		ZombiesRemaining = 5;
	}
}

void AEscapeGameMode::SpawnZombie()
{
	if (ZombiesOnMap < MaxZombiesOnMap)
	{
		if (ActiveZombieSpawnPoints.Num() < 1) return;

		int RandomIndex = FMath::RandRange(0, ActiveZombieSpawnPoints.Num() - 1);

		if (AZombieEscapeSpawnPoint* SpawnPoint = ActiveZombieSpawnPoints[RandomIndex])
		{
			FVector Loc = SpawnPoint->GetActorLocation();
			FRotator Rot = SpawnPoint->GetActorRotation();
			if (AZombieBase* Zombie = GetWorld()->SpawnActor<AZombieBase>(ZombieClass, Loc, Rot))
			{
				/*UE_LOG(LogTemp, Warning, TEXT("SPAWNING ZOMBIE, COUNT: %d"), ZombiesOnMap);*/
				++ZombiesOnMap;
			}
			
		}

	}
}

void AEscapeGameMode::NewZoneActive(uint8 ZoneNumber)
{
	for (AZombieEscapeSpawnPoint* SpawnPoint : ZombieSpawnPoints)
	{
		if (SpawnPoint && ZoneNumber == SpawnPoint->GetZone() && !SpawnPoint->IsActive())
		{
			ActiveZombieSpawnPoints.Add(SpawnPoint);
			SpawnPoint->Activate();
			if (!ActiveZones.Contains(ZoneNumber)) {
				ActiveZones.Add(ZoneNumber);
				if (ZoneNumber > 0) {
					IncrementMaxZombiesOnMap();
				}
			}
			/*UE_LOG(LogTemp, Warning, TEXT("Setting active zone %d"), ZoneNumber);
			UE_LOG(LogTemp, Warning, TEXT("MAX ZOMBIES ON MAP: %d"), MaxZombiesOnMap);*/
		}
	}
}

void AEscapeGameMode::DecrementZombiesOnMapCount()
{
	--ZombiesOnMap;
}

void AEscapeGameMode::PawnKilled(APawn* PawnKilled)
{
	APlayerController* PlayerController = Cast<APlayerController>(PawnKilled->GetController());
	if (PlayerController != nullptr)
	{
		PlayerController->GameHasEnded(PawnKilled, false);
	}
}

void AEscapeGameMode::IncrementMaxZombiesOnMap()
{
	FString MapName = GetWorld()->GetMapName();
	if (MapName.Contains("Dungeon")) {
		MaxZombiesOnMap += 6;
	}
	else
	{
		MaxZombiesOnMap += 3;
	}
	UE_LOG(LogTemp, Warning, TEXT("MAX ZOMBIES %d"), MaxZombiesOnMap);
}

