// Fill out your copyright notice in the Description page of Project Settings.


#include "Door.h"
#include "..\..\Public\Game\EscapeGameMode.h"
#include "..\..\Public\Player\CharacterBase.h"
#include "..\..\Usables\Inventory.h"
#include "..\..\Public\Weapon\Knife.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/AudioComponent.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

ADoor::ADoor()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("SkeletalMeshComponent");
	RootComponent = MeshComp;

	AccessZone = 0;
	bOpenDoor = false;
	bIsBarricade = false;
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();
	FindPressurePlate();
}

void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bOpenDoor || TotalMassOfActors() > MassToOpenDoors)
	{
		OpenDoor(DeltaTime);
		DoorLastOpened = GetWorld()->GetTimeSeconds();
	}
	else
	{
		if (GetWorld()->GetTimeSeconds() - DoorLastOpened > DoorCloseDelay)
		{
			CloseDoor(DeltaTime);
		}
	}
	
}



void ADoor::Use(ACharacterBase* Player)
{
	UInventory* Inventory = Player->GetInventoryComp();
	if (KeyToOpenDoor == nullptr || Inventory->ItemIsInInventory(KeyToOpenDoor))
	{
		bOpenDoor = !bOpenDoor;
	}
	else
	{
		UGameplayStatics::SpawnSoundAttached(LockedDoorSound, MeshComp, TEXT("LockedDoorSound"));
	}
}

uint8 ADoor::GetAccessZone()
{
	return AccessZone;
}


void ADoor::OpenDoor(float DeltaTime)
{
	if (bIsBarricade)
	{
		CurrentHeight = FMath::Lerp(CurrentHeight, OpenHeight, DeltaTime * DoorOpenSpeed);
		FVector DoorLocation = GetActorLocation();
		DoorLocation.Z = CurrentHeight;
		SetActorLocation(DoorLocation);
	}
	else
	{
		CurrentYaw = FMath::Lerp(CurrentYaw, OpenAngle, DeltaTime * DoorOpenSpeed);
		FRotator DoorRotation = GetActorRotation();
		DoorRotation.Yaw = CurrentYaw;
		SetActorRotation(DoorRotation);
	}

	bCloseDoorSoundFlag = false;
	if (!bOpenDoorSoundFlag)
	{
		UGameplayStatics::SpawnSoundAttached(OpenDoorSound, MeshComp, TEXT("OpenDorrSound"));
		bOpenDoorSoundFlag = true;
	}
	if (!bIsBarricade) {
		MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (AEscapeGameMode* GM = GetWorld()->GetAuthGameMode<AEscapeGameMode>())
	{
		GM->NewZoneActive(AccessZone);
	}
}


void ADoor::CloseDoor(float DeltaTime)
{
	if (bIsBarricade)
	{
		CurrentHeight = FMath::Lerp(CurrentHeight, InitialHeight, DeltaTime * DoorOpenSpeed);
		FVector DoorLocation = GetActorLocation();
		DoorLocation.Z = CurrentHeight;
		SetActorLocation(DoorLocation);
	}
	else
	{
		CurrentYaw = FMath::Lerp(CurrentYaw, InitialYaw, DeltaTime * DoorCloseSpeed);
		FRotator DoorRotation = GetActorRotation();
		DoorRotation.Yaw = CurrentYaw;
		SetActorRotation(DoorRotation);
	}

	bOpenDoorSoundFlag = false;
	if (!bCloseDoorSoundFlag)
	{
		UGameplayStatics::SpawnSoundAttached(CloseDoorSound, MeshComp, TEXT("CloseDoorSound"));
		bCloseDoorSoundFlag = true;
	}
}

float ADoor::TotalMassOfActors() const
{
	float TotalMass = 0.f;

	//// Find all overlapping actors
	TArray<AActor*> OverlappingActors;
	if (!PressurePlate) { return TotalMass; }
	PressurePlate->GetOverlappingActors(OUT OverlappingActors);

	//// Add up their masses
	for (AActor* Actor : OverlappingActors)
	{
		if(!Actor->IsA(AKnife::StaticClass()))
			TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
	}
	return TotalMass;
}

void ADoor::FindPressurePlate()
{
	if (!PressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("%s has the open door component on it but no pressurplate set!"));
	}

	if (!KeyToOpenDoor)
	{
		UE_LOG(LogTemp, Error, TEXT("%s has the open door component on it but no Key set!"));
	}
}

