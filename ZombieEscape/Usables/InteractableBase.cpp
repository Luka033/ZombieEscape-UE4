// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableBase.h"
#include "CharacterBase.h"


// Sets default values
AInteractableBase::AInteractableBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ObjectName = "Default";
	UIMessage = "Press and Hold F Interact with " + ObjectName;
	
}

void AInteractableBase::BeginPlay()
{
	Super::BeginPlay();
}

void AInteractableBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FString AInteractableBase::GetUIMessage()
{
	return UIMessage;
}

void AInteractableBase::Use(ACharacterBase* Player)
{
	
}

FString AInteractableBase::GetObjectName()
{
	return ObjectName;
}




