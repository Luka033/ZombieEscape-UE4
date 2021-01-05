// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Usables/InteractableBase.h"
#include "Usables/Pickup.h"
#include "Engine/TriggerVolume.h"
#include "Door.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIEESCAPE_API ADoor : public AInteractableBase
{
	GENERATED_BODY()
public:
	ADoor();


protected:
	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
	bool bIsBarricade;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
	class UStaticMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
	class UAnimationAsset* OpenAnimation;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
	USoundBase* OpenDoorSound;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
	USoundBase* CloseDoorSound;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
	USoundBase* LockedDoorSound;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
	uint8 AccessZone;


	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	FTimerHandle TDoorTimerHandle;

	virtual void Use(class ACharacterBase* Player) override;

public:
	uint8 GetAccessZone();

	void OpenDoor(float DeltaTime);
	void CloseDoor(float DeltaTime);
	float TotalMassOfActors() const;
	void FindPressurePlate();

	bool bOpenDoorSoundFlag = false;
	bool bCloseDoorSoundFlag = true;

private:
	UPROPERTY(EditAnywhere)
	float InitialYaw;

	UPROPERTY(EditAnywhere)
	float CurrentYaw;

	UPROPERTY(EditAnywhere)
	float OpenHeight = 200.0f;

	UPROPERTY(EditAnywhere)
	float InitialHeight;

	UPROPERTY(EditAnywhere)
	float CurrentHeight;

	bool bOpenDoor;

	UPROPERTY(EditAnywhere)
	float MassToOpenDoors = 30.f;

	UPROPERTY(EditAnywhere)
	float OpenAngle = 90.f;

	float DoorLastOpened = 0.f;

	UPROPERTY(EditAnywhere)
	float DoorCloseDelay = .2f;

	UPROPERTY(EditAnywhere)
	float DoorOpenSpeed = .8f;

	UPROPERTY(EditAnywhere)
	float DoorCloseSpeed = 2.f;

	UPROPERTY(EditAnywhere)
	ATriggerVolume* PressurePlate = nullptr;

	UPROPERTY(EditAnywhere)
	APickup* KeyToOpenDoor = nullptr;

};
