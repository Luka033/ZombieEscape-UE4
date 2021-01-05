// Fill out your copyright notice in the Description page of Project Settings.


#include "Barricade.h"
#include "..\..\Public\Player\CharacterBase.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/AudioComponent.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

ABarricade::ABarricade()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("SkeletalMeshComponent");
	RootComponent = MeshComp;

	MeshComp->SetRelativeLocation(StartLocation);
	bOpenDoor = false;
	OpenBarricadeSpeed = 0.8f;
}





void ABarricade::BeginPlay()
{
	Super::BeginPlay();
	FindPressurePlate();
}

void ABarricade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (TotalMassOfActors() > MassToOpenDoors)
	{
		UE_LOG(LogTemp, Warning, TEXT("OPENING DOOOOR"));
		OpenBarricade(DeltaTime);
		DoorLastOpened = GetWorld()->GetTimeSeconds();
	}
	
}

void ABarricade::FindPressurePlate()
{
	if (!PressurePlate || !PressurePlate2)
	{
		UE_LOG(LogTemp, Error, TEXT("%s has the open door component on it but no pressurplate set!"));
	}
}

void ABarricade::Use(ACharacterBase* Player)
{
	UE_LOG(LogTemp, Error, TEXT("OPENING BARRICADE"));
	bOpenDoor = true;
	/*GetWorld()->GetTimerManager().SetTimer(TLerpBarricade, this, &ABarricade::OpenBarricade, 0.01f, true);*/

}

void ABarricade::OpenBarricade(float DeltaTime)
{
	CurrentHeight = FMath::Lerp(CurrentHeight, OpenHeight, DeltaTime * OpenBarricadeSpeed);
	FVector DoorLocation = GetActorLocation();
	DoorLocation.Z = CurrentHeight;
	SetActorLocation(DoorLocation);
}

float ABarricade::TotalMassOfActors() const
{
	float TotalMass = 0.f;

	//// Find all overlapping actors
	TArray<AActor*> OverlappingActors;
	/*if (!PressurePlate) { return TotalMass; }
	if (!PressurePlate2) { return TotalMass; }*/
	
	if(PressurePlate)
		PressurePlate->GetOverlappingActors(OUT OverlappingActors);
	if(PressurePlate2)
		PressurePlate2->GetOverlappingActors(OUT OverlappingActors);

	//// Add up their masses
	for (AActor* Actor : OverlappingActors)
	{
		TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
	}
	return TotalMass;
}
