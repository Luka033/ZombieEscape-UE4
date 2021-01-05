// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/WeaponBase.h"
#include "WeaponFull.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIEESCAPE_API AWeaponFull : public AWeaponBase
{
	GENERATED_BODY()

public:
	AWeaponFull();

protected:
	bool bIsInFullAuto;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
	bool bIsSelectFire;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
		float FireRate;

	/*UPROPERTY(Replicated)*/
	bool bIsFiring;

	UFUNCTION()
	void OnRep_StartFullAutoFire();

	FTimerHandle WeaponFireHandle;
	void PlayWeaponEffects();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Fire() override;
	virtual void StopFiring() override;
	virtual void ChangeFireMode() override;
	virtual bool IsFiring() override { return bIsFiring; }
};
