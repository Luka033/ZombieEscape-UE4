// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup.h"
#include "Inventory.h"
#include "..\..\Public\Player\CharacterBase.h"
#include "..\..\Public\Weapon\WeaponBase.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Texture2d.h"
#include "Kismet/GameplayStatics.h"

APickup::APickup()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	RootComponent = MeshComp;
}


void APickup::BeginPlay()
 {
	Super::BeginPlay();
	
}


void APickup::Use(ACharacterBase* Player)
{
	if (PickupType == EPickupType::EAmmo)
	{
		AWeaponBase* CurrentWeapon = Player->GetCurrentWeapon();
		CurrentWeapon->HasFullAmmo();
		if (CurrentWeapon->HasFullAmmo()) { return; }
		CurrentWeapon->IncreaseAmmoAmount();
	}
	else if (PickupType == EPickupType::EHealth)
	{
		if (Player->GetHealth() >= 100) { return; }
		Player->SetHealth();
	}
	else if (PickupType == EPickupType::EKey)
	{
		Player->Inventory->AddItem(this);
	}
	UGameplayStatics::SpawnSoundAttached(PickupItemSound, MeshComp, TEXT("PickupItemSound"));
	MeshComp->SetHiddenInGame(true);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

APickup* APickup::GetItem()
{
	return this;
}



