// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"
#include "ZombieEscapeProjectile.h"
#include "WeaponBase.h"
#include "..\..\Public\Weapon\WeaponBase.h"
#include "ZombieBase.h"
#include "..\..\Usables\Inventory.h"
#include "Knife.h"
#include "..\..\Usables\Interacter.h"
#include "..\..\Public\Weapon\Knife.h"
#include "..\..\Public\Game\EscapeGameMode.h"



#include "ZombieEscapeProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include "..\..\Public\Player\CharacterBase.h"
#include "DrawDebugHelpers.h"
#include "Animation/AnimInstance.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ACharacterBase::ACharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->GetRelativeLocation() = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->GetRelativeRotation() = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->GetRelativeLocation() = FVector(-0.5f, -4.4f, -155.7f);

	Inventory = CreateDefaultSubobject<UInventory>(TEXT("InventoryComponent"));

	Health = 100;
	WeaponIndex = 0;
	bIsAiming = false;
	RKnifeAttack = 0;
	bIsPerformingAction = false;
}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		// Spawn weapon using startingweapon class
		
		if (KnifeClass)
		{
			Knife = GetWorld()->SpawnActor<AKnife>(KnifeClass, SpawnParams);
			Knife->AttachToComponent(Mesh1P, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("s_knifeHolster"));
		}
		CurrentWeapon = GetWorld()->SpawnActor<AWeaponBase>(StartingWeaponClass, SpawnParams);
		if (CurrentWeapon)
		{
			/*CurrentWeapon->SetOwner(this);*/
			WeaponArray.Add(CurrentWeapon);
			OnRep_AttachWeapon();
		}
		GetWorld()->GetTimerManager().SetTimer(TAimHandle, this, &ACharacterBase::AimCameraFOV, 0.005f, true);
		GetWorld()->GetTimerManager().PauseTimer(TAimHandle);
	}
}

// Called to bind functionality to input
void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind aim events
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ACharacterBase::OnAimingStart);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ACharacterBase::OnAimingEnd);

	// Bind fire event and reload
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ACharacterBase::OnFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ACharacterBase::OnStopFire);
	PlayerInputComponent->BindAction("SelectFireWeaponToggle", IE_Pressed, this, &ACharacterBase::ChangeWeaponFireMode);
	PlayerInputComponent->BindAction("SwitchNextWeapon", IE_Pressed, this, &ACharacterBase::SwitchNextWeapon);
	PlayerInputComponent->BindAction("SwitchPreviousWeapon", IE_Pressed, this, &ACharacterBase::SwitchPreviousWeapon);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ACharacterBase::OnReload);
	PlayerInputComponent->BindAction("KnifeAttack", IE_Pressed, this, &ACharacterBase::OnKnifeAttack);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ACharacterBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACharacterBase::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ACharacterBase::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ACharacterBase::LookUpAtRate);
}

void ACharacterBase::GivePlayerWeapon(AWeaponBase* Weapon)
{
	if (Weapon)
	{
		if (WeaponArray.Num() >= 2)
		{
			WeaponArray[WeaponIndex]->Destroy();
			WeaponArray[WeaponIndex] = Weapon;
		}
		else
		{
			WeaponArray.Add(Weapon);
			WeaponIndex = WeaponArray.Num() - 1;
		}
		CurrentWeapon = Weapon;
		OnRep_AttachWeapon();
	}
}

void ACharacterBase::OnRep_AttachWeapon()
{
	if (CurrentWeapon)
	{
		if (IsLocallyControlled())
		{
			CurrentWeapon->AttachToComponent(Mesh1P, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("s_weaponSocket"));
		}
		else
		{
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("s_weaponSocket"));
		}
	}
}

void ACharacterBase::SwitchNextWeapon()
{
	//UE_LOG(LogTemp, Warning, TEXT("Switching to next weapon"));
	if (CurrentWeapon)
	{
		if (CurrentWeapon->IsFiring()) { return; }
		if (WeaponArray.Num() > WeaponIndex + 1)
		{
			++WeaponIndex;
			if (AWeaponBase* NextWeapon = WeaponArray[WeaponIndex])
			{
				CurrentWeapon->SetActorHiddenInGame(true);
				CurrentWeapon = NextWeapon;
				CurrentWeapon->SetActorHiddenInGame(false);
			}
		}
	}
}

void ACharacterBase::SwitchPreviousWeapon()
{
	/*UE_LOG(LogTemp, Warning, TEXT("Switching to previous weapon"));*/
	if (CurrentWeapon)
	{
		if (CurrentWeapon->IsFiring()) { return; }
		if (WeaponIndex - 1 >= 0)
		{
			--WeaponIndex;
			if (AWeaponBase* NextWeapon = WeaponArray[WeaponIndex])
			{
				CurrentWeapon->SetActorHiddenInGame(true);
				CurrentWeapon = NextWeapon;
				CurrentWeapon->SetActorHiddenInGame(false);
			}
		}
	}
}

void ACharacterBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACharacterBase, CurrentWeapon);
}

void ACharacterBase::DecrementHealth(int16 Damage)
{
	if (HasAuthority())
	{
		Health -= Damage;
		/*bCanBeDamaged = false;*/
		/*UE_LOG(LogTemp, Warning, TEXT("Player hit, health left: %d"), Health);*/

		OnHealthChanged.Broadcast(Health);
		GetWorld()->GetFirstPlayerController()->ClientPlayCameraShake(HitShake);
		
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), TakeDamageSound, GetActorLocation());
		if (Health <= 0)
		{	
			// perform death
			AEscapeGameMode* GameMode = GetWorld()->GetAuthGameMode<AEscapeGameMode>();
			if (GameMode != nullptr)
			{
				GameMode->PawnKilled(this);
			}
			DetachFromControllerPendingDestroy();
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}


void ACharacterBase::OnFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Fire(); // if fire valid
	}
}

void ACharacterBase::OnStopFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFiring();
	}
}

void ACharacterBase::ChangeWeaponFireMode()
{
	if (CurrentWeapon)
		CurrentWeapon->ChangeFireMode();
}

void ACharacterBase::OnReload()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Reload();
	}
}


void ACharacterBase::OnAimingStart()
{
	/*if (!IsPerformingAction()) {*/
		GetWorld()->GetTimerManager().UnPauseTimer(TAimHandle);
		UCharacterMovementComponent* MovementComp = Cast<UCharacterMovementComponent>(this->GetCharacterMovement());
		MovementComp->MaxWalkSpeed = 200.0f;
		bIsAiming = true;
	
}

void ACharacterBase::OnAimingEnd()
{
	GetWorld()->GetTimerManager().UnPauseTimer(TAimHandle);
	UCharacterMovementComponent* MovementComp = Cast<UCharacterMovementComponent>(this->GetCharacterMovement());
	UInteracter* Interacter = Cast<UInteracter>(this->GetComponentByClass(UInteracter::StaticClass()));
	if (Interacter != nullptr && !Interacter->IsCarryingItem())
		MovementComp->MaxWalkSpeed = 600.0f;
	bIsAiming = false;
}

void ACharacterBase::AimCameraFOV()
{
	float CameraFOV = FirstPersonCameraComponent->FieldOfView;
	float NewFOV = 90.0f;
	if (bIsAiming)
		NewFOV = 70.0f;
	NewFOV = UKismetMathLibrary::FInterpTo_Constant(CameraFOV, NewFOV, GetWorld()->DeltaTimeSeconds, 25.0f);

	FirstPersonCameraComponent->SetFieldOfView(NewFOV);
}

bool ACharacterBase::GetIsAiming()
{
	return bIsAiming;
}

void ACharacterBase::OnKnifeAttack()
{
	if (Knife)
	{
		Knife->OnKnife();
	}
}

AWeaponBase* ACharacterBase::GetCurrentWeapon()
{
	return CurrentWeapon;
}

FString ACharacterBase::GetWeaponAmmo() const
{
	if (CurrentWeapon)
	{
		TArray<int32> CurrentAmmo = CurrentWeapon->GetCurrentAmmo();
		return FString::FromInt(CurrentAmmo[0]) + FString("/") + FString::FromInt(CurrentAmmo[1]);
	}
	else
	{
		return FString("");
	}
}

uint8 ACharacterBase::GetWeaponArrayCount()
{
	return WeaponArray.Num();
}

void ACharacterBase::SetHealth()
{
	Health = 100;
}

int16 ACharacterBase::GetHealth()
{
	return Health;
}

float ACharacterBase::GetHealthPercent() const
{
	return Health / 100.0f;
}

UInventory* ACharacterBase::GetInventoryComp()
{
	return Inventory;
}






void ACharacterBase::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ACharacterBase::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ACharacterBase::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ACharacterBase::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}



