// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHealthChanged, int, Health);

UCLASS()
class ZOMBIEESCAPE_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()


public:
	// Sets default values for this character's properties
	ACharacterBase();
	

public:
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty>& OutLifetimeProps) const;

	UFUNCTION(BlueprintPure)
	float GetHealthPercent() const;

	UFUNCTION(BlueprintPure)
	FString GetWeaponAmmo() const;

	void SetHealth();
	int16 GetHealth();
	void DecrementHealth(int16 Damage);
	uint8 GetWeaponArrayCount();

protected:

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* Mesh1P;
	
	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
	TSubclassOf<class AWeaponBase> StartingWeaponClass;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
	TSubclassOf<class AWeaponBase> SecondWeaponClass;

	UPROPERTY(ReplicatedUsing = OnRep_AttachWeapon)
	class AWeaponBase* CurrentWeapon;

	UFUNCTION()
	void OnRep_AttachWeapon();

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
	TSubclassOf<class AKnife> KnifeClass;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE class AKnife* GetKnife() { return Knife; }

	class AKnife* Knife;

	UPROPERTY(Replicated)
	uint8 RKnifeAttack;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
	USoundBase* TakeDamageSound;

	UPROPERTY(BlueprintAssignable)
	FHealthChanged OnHealthChanged;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
	TSubclassOf<UCameraShake> HitShake;

	// set to replicate, skip owner
	bool bIsAiming;
	FTimerHandle TAimHandle;
	int16 Health;
	bool bIsPerformingAction;
	int32 WeaponIndex;
	TArray<AWeaponBase*> WeaponArray;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Fires a projectile. */
	virtual void OnFire();
	virtual void OnStopFire();
	virtual void ChangeWeaponFireMode();
	void OnReload();
	virtual void OnAimingStart();
	virtual void OnAimingEnd();
	void AimCameraFOV();
	void OnKnifeAttack();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

public:
	void SwitchNextWeapon();
	void SwitchPreviousWeapon();

	UFUNCTION(BlueprintPure)
	bool GetIsAiming();

	UFUNCTION(BlueprintCallable)
	class AWeaponBase* GetCurrentWeapon();

	TArray<AWeaponBase*>* GetWeaponArray() { return &WeaponArray; }

	void GivePlayerWeapon(class AWeaponBase* Weapon);

	class UInventory* Inventory;

	UFUNCTION(BlueprintCallable)
	class UInventory* GetInventoryComp();

	UFUNCTION(BlueprintCallable)
	void SetPerformingAction(bool IsPerformingAction) { bIsPerformingAction = IsPerformingAction; }

	UFUNCTION(BlueprintCallable)
	bool IsPerformingAction() { return bIsPerformingAction; }

	/** Returns Mesh1P subobject **/
	UFUNCTION(BlueprintCallable)
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};
