// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Usables/InteractableBase.h"
#include "Pickup.generated.h"


UENUM(BlueprintType)
enum class EPickupType : uint8
{
	EHealth UMETA(DisplayName="Health"),
	EAmmo UMETA(DisplayName="Ammo"),
	EKey UMETA(DisplayName = "Key"),

};

UCLASS()
class ZOMBIEESCAPE_API APickup : public AInteractableBase
{
	GENERATED_BODY()

public:
	APickup();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
		class UStaticMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere, Category = "Enums")
		EPickupType PickupType;

	virtual void Use(class ACharacterBase* Player) override;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
		USoundBase* PickupItemSound;

	

public:

	APickup* GetItem();
	
};
