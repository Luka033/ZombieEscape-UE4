// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponFull.h"
#include "..\..\Public\Weapon\WeaponFull.h"
#include "CharacterBase.h"
#include "ZombieBase.h"

#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

AWeaponFull::AWeaponFull()
{
	WeaponMaxAmmo = 180;
	MagazineMaxAmmo = 30;
	bIsInFullAuto = true;
	bIsSelectFire = true;
	bIsFiring = false;
	FireRate = 500;
}



void AWeaponFull::BeginPlay()
{
	Super::BeginPlay();
	CurrentTotalAmmo = WeaponMaxAmmo / 3;
	CurrentMagazineAmmo = MagazineMaxAmmo;

	FireRate = 60 / FireRate;
}

//void AWeaponFull::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//	DOREPLIFETIME_CONDITION(AWeaponFull, bIsFiring, COND_OwnerOnly);
//}

void AWeaponFull::OnRep_StartFullAutoFire()
{
	if(bIsFiring)
		GetWorld()->GetTimerManager().SetTimer(WeaponFireHandle, this, &AWeaponFull::PlayWeaponEffects, FireRate, true);
	else
		GetWorld()->GetTimerManager().ClearTimer(WeaponFireHandle);
}

void AWeaponFull::PlayWeaponEffects()
{
	if (CurrentMagazineAmmo > 0)
	{
		if (ACharacterBase* ShootingPlayer = Cast<ACharacterBase>(GetOwner()))
		{
			--CurrentMagazineAmmo;
			UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, WeaponMesh, TEXT("muzzleSocket"));
			GetWorld()->GetFirstPlayerController()->ClientPlayCameraShake(FireShake);
			if (UAnimInstance* AnimInstance = ShootingPlayer->GetMesh1P()->GetAnimInstance())
			{
				if (FPSArmsMontage)
				{
					AnimInstance->Montage_Play(FPSArmsMontage);
					if (ShootingPlayer->GetIsAiming())
						AnimInstance->Montage_JumpToSection(FName("FireADS"), FPSArmsMontage);
					else
						AnimInstance->Montage_JumpToSection(FName("FireHip"), FPSArmsMontage);

				}
			}

			if (CurrentMagazineAmmo <= 0 && FireEmptyAnimation)
				WeaponMesh->PlayAnimation(FireEmptyAnimation, false);
			else if (FireAnimation)
				WeaponMesh->PlayAnimation(FireAnimation, false);

			TArray<FHitResult> HitResults = PerformLineTrace(ShootingPlayer);
			if (HitResults.Num() > 0)
			{
				float PenetrationDamageMultiplier = 1.0f;
				for (FHitResult& Result : HitResults)
				{
					FString HitBone = Result.BoneName.ToString();
					if (AActor* HitActor = Result.GetActor())
					{
						FVector Location;
						FRotator Rotation;
						ShootingPlayer->GetController()->GetPlayerViewPoint(Location, Rotation);
						FVector ShotDirection = -Rotation.Vector();
						if (AZombieBase* Zombie = Cast<AZombieBase>(HitActor))
						{
							/*UE_LOG(LogTemp, Warning, TEXT("\nTHIS BULELT HIT: %s"), *Zombie->GetName());
							UE_LOG(LogTemp, Warning, TEXT("PENETRATION MULTIPLIER: %f"), PenetrationDamageMultiplier);*/
							Zombie->Hit(ShootingPlayer, Result, PenetrationDamageMultiplier);
							PenetrationDamageMultiplier /= 2.0f;
							UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BodyImpactEffect, Result.Location, ShotDirection.Rotation());
						}
						else
						{
							UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MiscImpactEffect, Result.Location, ShotDirection.Rotation());
						}
					}
				}
			}
		}
	}
	else if (CurrentMagazineAmmo <= 0)
	{
		UGameplayStatics::SpawnSoundAttached(EmptyGunSound, WeaponMesh, TEXT("muzzleSocket"));
	}
}



void  AWeaponFull::Fire()
{
	if (!bIsFiring && bIsInFullAuto && bCanFire) // and is in full auto
	{	

		if (ACharacterBase* Player = Cast<ACharacterBase>(GetOwner()))
			if (Player->IsPerformingAction()) { return; }

		bIsFiring = true;
		bCanFire = false;
		FTimerHandle FireRateHandle;
		GetWorld()->GetTimerManager().SetTimer(FireRateHandle, this, &AWeaponFull::ControlFireDelay, DelayBetweenShots, false);

		PlayWeaponEffects();
		GetWorld()->GetTimerManager().SetTimer(WeaponFireHandle, this, &AWeaponFull::PlayWeaponEffects, FireRate, true);
		
	}
	else // is in semi auto
	{
		Super::Fire();
	}
}

void AWeaponFull::StopFiring()
{
	GetWorld()->GetTimerManager().ClearTimer(WeaponFireHandle);
	bIsFiring = false;

}

void AWeaponFull::ChangeFireMode()
{
	if (bIsSelectFire)
		bIsInFullAuto = !bIsInFullAuto;
}
