// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "CharacterBase.h"
#include "ZombieBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "..\..\Public\Weapon\WeaponBase.h"
#include "DrawDebugHelpers.h"
#include "Animation/AnimInstance.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AWeaponBase::AWeaponBase()
{

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	RootComponent = WeaponMesh;
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SetReplicates(true);

	WeaponName = "Default Name";
	WeaponMaxAmmo = 180;
	MagazineMaxAmmo = 30;

	CurrentTotalAmmo = WeaponMaxAmmo / 3;
	CurrentMagazineAmmo = MagazineMaxAmmo;
	DelayBetweenShots = 0.15;
	bCanFire = true;
	bIsFiring = false;
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	WeaponMesh->HideBoneByName(FName("emptyCase_1"), EPhysBodyOp::PBO_None);
	WeaponMesh->HideBoneByName(FName("emptyCase_2"), EPhysBodyOp::PBO_None);
	WeaponMesh->HideBoneByName(FName("emptyCase_3"), EPhysBodyOp::PBO_None);
	WeaponMesh->HideBoneByName(FName("emptyCase_4"), EPhysBodyOp::PBO_None);
	WeaponMesh->HideBoneByName(FName("Magazine2"), EPhysBodyOp::PBO_None);
	
}

void AWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AWeaponBase, CurrentTotalAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AWeaponBase, CurrentMagazineAmmo, COND_OwnerOnly);

}

TArray<FHitResult> AWeaponBase::PerformLineTrace(class ACharacterBase* ShootingPlayer)
{
	FVector Start = WeaponMesh->GetSocketLocation(FName("muzzleSocket"));
	FVector Rot = WeaponMesh->GetSocketRotation(FName("muzzleSocket")).Vector();
	FVector End = Start + Rot * 100000;

	TArray<FHitResult> HitResults;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	CollisionParams.AddIgnoredActor(ShootingPlayer);
	FCollisionResponseParams CollisionRespone;


	GetWorld()->LineTraceMultiByChannel(OUT HitResults, Start, End, ECollisionChannel::ECC_GameTraceChannel2, CollisionParams, CollisionRespone);
	/*DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.0f, 0, 3.0f);*/
	return HitResults;
	
}

void AWeaponBase::ControlFireDelay()
{
	bCanFire = true;
	bIsFiring = false;
	if (ACharacterBase* Player = Cast<ACharacterBase>(GetOwner()))
		Player->SetPerformingAction(false);
}

void AWeaponBase::Fire()
{
	if (ACharacterBase* Player = Cast<ACharacterBase>(GetOwner()))
	{
		if (Player->IsPerformingAction()) { return; }
		if (CurrentMagazineAmmo > 0 && bCanFire)
		{
			UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, WeaponMesh, TEXT("muzzleSocket"));
			GetWorld()->GetFirstPlayerController()->ClientPlayCameraShake(FireShake);
			if (UAnimInstance* AnimInstance = Player->GetMesh1P()->GetAnimInstance())
			{
				if (FPSArmsMontage)
				{
					AnimInstance->Montage_Play(FPSArmsMontage);
					if (Player->GetIsAiming())
						AnimInstance->Montage_JumpToSection(FName("FireADS"), FPSArmsMontage);
					else
						AnimInstance->Montage_JumpToSection(FName("FireHip"), FPSArmsMontage);

				}
			}

			--CurrentMagazineAmmo;
			bCanFire = false;
			bIsFiring = true;

			if (CurrentMagazineAmmo <= 0 && FireEmptyAnimation)
				WeaponMesh->PlayAnimation(FireEmptyAnimation, false);
			else if (FireAnimation)
				WeaponMesh->PlayAnimation(FireAnimation, false);

			TArray<FHitResult> HitResults = PerformLineTrace(Player);
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
						Player->GetController()->GetPlayerViewPoint(Location, Rotation);
						FVector ShotDirection = -Rotation.Vector();
						if (AZombieBase* Zombie = Cast<AZombieBase>(HitActor))
						{
							Zombie->Hit(Player, Result, PenetrationDamageMultiplier);
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
			FTimerHandle FireRateHandle;
			GetWorld()->GetTimerManager().SetTimer(FireRateHandle, this, &AWeaponBase::ControlFireDelay, DelayBetweenShots, false);
		}
		else if (CurrentMagazineAmmo <= 0)
		{			
			UGameplayStatics::SpawnSoundAttached(EmptyGunSound, WeaponMesh, TEXT("muzzleSocket"));
		}
	}
}

void AWeaponBase::StopFiring()
{

}

void AWeaponBase::ChangeFireMode()
{
}

FWeaponDamage AWeaponBase::GetWeaponDamage()
{
	return WeaponDamage;
}

void AWeaponBase::Reload()
{
	if (CurrentTotalAmmo > 0 && CurrentMagazineAmmo != MagazineMaxAmmo)
	{
		if (ACharacterBase* Player = Cast<ACharacterBase>(GetOwner())) {

			if (Player->IsPerformingAction()) { return; }

			bool bMagasineIsEmpty = CurrentMagazineAmmo <= 0;

			if (UAnimInstance* AnimInstance = Player->GetMesh1P()->GetAnimInstance())
			{
				if (FPSArmsMontage)
				{
					bCanFire = false;
					AnimInstance->Montage_Play(FPSArmsMontage);
					float AnimationLength = 5.0f;
					if (bMagasineIsEmpty)
					{
						AnimInstance->Montage_JumpToSection(FName("ReloadEmpty"), FPSArmsMontage);
						AnimationLength = FPSArmsMontage->GetSectionLength(4);
					}
					else
					{		
						AnimInstance->Montage_JumpToSection(FName("Reload"), FPSArmsMontage);
						AnimationLength = FPSArmsMontage->GetSectionLength(3);
					}
					FTimerHandle FireRateHandle;
					GetWorld()->GetTimerManager().SetTimer(FireRateHandle, this, &AWeaponBase::ControlFireDelay, AnimationLength, false);
				}
			}

			if (CurrentMagazineAmmo <= 0 && ReloadEmptyAnimation)
				WeaponMesh->PlayAnimation(ReloadEmptyAnimation, false);
			else if (ReloadAnimation)
				WeaponMesh->PlayAnimation(ReloadAnimation, false);

			int Difference = MagazineMaxAmmo - CurrentMagazineAmmo;
			if (CurrentTotalAmmo - Difference >= 0)
			{
				CurrentTotalAmmo -= Difference;
				CurrentMagazineAmmo = MagazineMaxAmmo;
			}
			else
			{
				CurrentMagazineAmmo += CurrentTotalAmmo;
				CurrentTotalAmmo = 0;
			}

			Player->SetPerformingAction(true);	
		}
	}
}



TArray<int32> AWeaponBase::GetCurrentAmmo()
{
	TArray<int32> Ammo = {};
	return { CurrentMagazineAmmo, CurrentTotalAmmo };
}

void AWeaponBase::IncreaseAmmoAmount()
{
	if (2 * MagazineMaxAmmo + CurrentTotalAmmo >= WeaponMaxAmmo)
		CurrentTotalAmmo = WeaponMaxAmmo;
	else
		CurrentTotalAmmo += (2 * MagazineMaxAmmo);
}

int32 AWeaponBase::GetMagazineAmmo()
{
	return CurrentMagazineAmmo;
}

bool AWeaponBase::HasFullAmmo()
{
	return CurrentTotalAmmo == WeaponMaxAmmo;
}

UAnimMontage* AWeaponBase::GetFPSAnimMontage()
{
	return FPSArmsMontage;
}

TEnumAsByte<EWeaponID> AWeaponBase::GetWeaponID()
{
	return WeaponID;
}



