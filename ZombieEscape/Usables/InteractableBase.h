// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableBase.generated.h"

UCLASS()
class ZOMBIEESCAPE_API AInteractableBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractableBase();

	

protected:

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
	FString UIMessage;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
	FString ObjectName;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;


public:
	FString GetUIMessage();
	virtual void Use(class ACharacterBase* Player);

	FString GetObjectName();
	

};
