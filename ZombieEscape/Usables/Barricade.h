// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Usables/InteractableBase.h"
#include "Engine/TriggerVolume.h"
#include "Barricade.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIEESCAPE_API ABarricade : public AInteractableBase
{
	GENERATED_BODY()

public:
	ABarricade();

public:
	void FindPressurePlate();

protected:
	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
		class UStaticMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
		USoundBase* OpenDoorSound;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
		USoundBase* CloseDoorSound;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
		FVector StartLocation;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
		FVector EndLocation;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
		float OpenBarricadeSpeed;

	void OpenBarricade(float DeltaTime);

	float TotalMassOfActors() const;

	FTimerHandle TLerpBarricade;



	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void Use(class ACharacterBase* Player) override;


private:
	bool bOpenDoor;

	float DoorLastOpened = 0.f;

	UPROPERTY(EditAnywhere)
		float MassToOpenDoors = 30.f;

	

	UPROPERTY(EditAnywhere)
		ATriggerVolume* PressurePlate = nullptr;

	UPROPERTY(EditAnywhere)
		ATriggerVolume* PressurePlate2 = nullptr;

	UPROPERTY(EditAnywhere)
		float OpenHeight = 200.0f;

	UPROPERTY(EditAnywhere)
		float InitialHeight;

	UPROPERTY(EditAnywhere)
		float CurrentHeight;
	
};
