// Fill out your copyright notice in the Description page of Project Settings.


#include "WallWeapon.h"
#include "..\..\Public\Weapon\WeaponBase.h"
#include "..\..\Public\Player\CharacterBase.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"

AWallWeapon::AWallWeapon()
{

	/*WallWeaponOutline = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");*/
	WallWeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMeshComponent");
	
	RootComponent = WallWeaponMesh;

	WallWeaponMesh->HideBoneByName(FName("Magazine2"), EPhysBodyOp::PBO_None);
}


void AWallWeapon::Use(ACharacterBase* Player)
{
	if (Player)
	{

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = Player;
		if (AWeaponBase* Weapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, SpawnParams))
		{
			USkeletalMesh* WeaponMeshToDrop = Cast<USkeletalMesh>(Player->GetCurrentWeapon()->GetWeaponMesh()->SkeletalMesh);
			AWeaponBase* WeaponClassToDrop = Player->GetCurrentWeapon();


			uint8 PlayerWeaponCount = Player->GetWeaponArrayCount();
			Player->GivePlayerWeapon(Weapon);
			if (PlayerWeaponCount == 1)
			{
				Player->SwitchPreviousWeapon();
				Player->SwitchNextWeapon();
			}
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickupWeaponSound, Player->GetActorLocation());
			
			
			/*UE_LOG(LogTemp, Warning, TEXT("PLAYER HAS %d WEAPONS"), Player->GetWeaponArrayCount());*/
			if (PlayerWeaponCount > 1)
			{
				if (WeaponMeshToDrop)
				{
					WallWeaponMesh->SetSkeletalMesh(WeaponMeshToDrop);
					WeaponClass = WeaponClassToDrop->GetClass();
					/*UE_LOG(LogTemp, Warning, TEXT("Switched Weapons sucessfully!===================="));*/
				}
			}
			else
			{
				this->Destroy();
				/*UE_LOG(LogTemp, Warning, TEXT("WEAPON DESTROYED!!!!"))*/
			}
			WallWeaponMesh->HideBoneByName(FName("Magazine2"), EPhysBodyOp::PBO_None);
			
			
				
		}
	}
}
