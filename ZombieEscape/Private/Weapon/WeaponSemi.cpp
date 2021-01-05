// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponSemi.h"
#include "..\..\Public\Weapon\WeaponSemi.h"
#include "CharacterBase.h"
#include "ZombieBase.h"

#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

AWeaponSemi::AWeaponSemi()
{
	WeaponName = "Default Name";
	WeaponMaxAmmo = 42;
	MagazineMaxAmmo = 7;	
}

void AWeaponSemi::BeginPlay()
{
	Super::BeginPlay();
	CurrentTotalAmmo = WeaponMaxAmmo / 3;
	CurrentMagazineAmmo = MagazineMaxAmmo;
}

