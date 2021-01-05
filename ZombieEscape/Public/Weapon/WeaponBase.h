// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

UENUM(BlueprintType)
enum EHitLocation
{
	None UMETA(DisplayName, "None"),
	Head UMETA(DisplayName, "Head"),
	Neck UMETA(DisplayName, "Neck"),
	Chest UMETA(DisplayName, "Chest"),
	Abdomen UMETA(DisplayName, "Abdomen")

};

USTRUCT(BlueprintType)
struct FWeaponDamage
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
		float BaseDamage = 20.0f;
	UPROPERTY(EditDefaultsOnly)
		float HeadMultiplier = 3.5f;
	UPROPERTY(EditDefaultsOnly)
		float ChestMultiplier = 1.25f;
	UPROPERTY(EditDefaultsOnly)
		float AbdomenMultiplier = 1.1f;


	float GetDamage(EHitLocation HitLocation)
	{
		switch (HitLocation)
		{
		case Head: return BaseDamage * HeadMultiplier;
		case Chest: return BaseDamage * ChestMultiplier;
		case Abdomen: return BaseDamage * AbdomenMultiplier;
		default: return BaseDamage;
		}
	}
};


UENUM(BlueprintType)
enum EWeaponID
{
	Colt1911 UMETA(DisplayName, "1911"),
	M1Carbine UMETA(DisplayName, "M1Carbine"),
	STG44 UMETA(DisplayName, "STG44"),
	AK47 UMETA(DisplayName, "AK47"),
};

UCLASS()
class ZOMBIEESCAPE_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TArray<FHitResult> PerformLineTrace(class ACharacterBase* ShootingPlayer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
	TEnumAsByte<EWeaponID> WeaponID;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
	class UAnimationAsset* FireAnimation;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
	class UAnimationAsset* FireEmptyAnimation;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
	class UAnimationAsset* ReloadAnimation;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
	class UAnimationAsset* ReloadEmptyAnimation;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
	class UAnimMontage* FPSArmsMontage;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
	class UAnimMontage* ThirdPersonMontage;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
	TSubclassOf<UCameraShake> FireShake;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
	UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
	UParticleSystem* MiscImpactEffect;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
	UParticleSystem* BodyImpactEffect;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
	FString WeaponName;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
	float DelayBetweenShots;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
	FWeaponDamage WeaponDamage;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
	int32 WeaponMaxAmmo;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
	int32 MagazineMaxAmmo;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
	USoundBase* EmptyGunSound;

	bool bCanFire;
	void ControlFireDelay();

	UPROPERTY(Replicated)
	int32 CurrentTotalAmmo;
	UPROPERTY(Replicated)
	int32 CurrentMagazineAmmo;

private:
	bool bIsFiring;

public:	
	virtual void Fire();
	virtual void StopFiring();
	virtual void ChangeFireMode();
	FWeaponDamage GetWeaponDamage();
	virtual bool IsFiring() { return bIsFiring; }
	

	void IncreaseAmmoAmount();

	// 0 = cant reload, 1 = Reload, 2 = ReloadEmpty
	virtual void Reload();

	// first element is magazine ammo, second element is total ammo
	TArray<int32> GetCurrentAmmo();
	int32 GetMagazineAmmo();
	bool HasFullAmmo();
	class UAnimMontage* GetFPSAnimMontage();

	UFUNCTION(BlueprintCallable)
		TEnumAsByte<EWeaponID> GetWeaponID();

	class USkeletalMeshComponent* GetWeaponMesh() { return WeaponMesh; }

};

