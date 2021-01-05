// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "WinTriggerBox.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIEESCAPE_API AWinTriggerBox : public ATriggerBox
{
	GENERATED_BODY()

public:
	AWinTriggerBox();

protected:
	virtual void BeginPlay() override;


public:
	UFUNCTION()
	void OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);	
};
