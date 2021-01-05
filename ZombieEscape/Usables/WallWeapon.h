// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Usables/InteractableBase.h"
#include "WallWeapon.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIEESCAPE_API AWallWeapon : public AInteractableBase
{
	GENERATED_BODY()

public:
	AWallWeapon();

protected:

	//UPROPERTY(EditAnywhere, Category = "EscapeSettings")
	//	class UStaticMeshComponent* WallWeaponOutline;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
	class USkeletalMeshComponent* WallWeaponMesh;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
	TSubclassOf<class AWeaponBase> WeaponClass;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
		USoundBase* PickupWeaponSound;

public:
	virtual void Use(class ACharacterBase* Player) override;

	
};
