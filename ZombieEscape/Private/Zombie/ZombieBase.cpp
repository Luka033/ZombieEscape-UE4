// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieBase.h"
#include "..\..\Public\Zombie\ZombieBase.h"
#include "CharacterBase.h"
#include "WeaponBase.h"
#include "..\..\Public\Weapon\WeaponBase.h"
#include "..\..\Public\Weapon\Knife.h"
#include "..\..\Public\Game\EscapeGameMode.h"


#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"

// Sets default values
AZombieBase::AZombieBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Health = 150.0f;
	bIsDead = false;
	CleanupDelay = 10.0f;
	bCanDoDamage = true;
	bInRangeToAttack = false;
	MovementSpeeds = { 20, 60, 80 };

	LeftHandCollisionComponent = CreateDefaultSubobject<UBoxComponent>("LeftHandCollisionComponent");
	LeftHandCollisionComponent->SetupAttachment(RootComponent);

	RightHandCollisionComponent = CreateDefaultSubobject<UBoxComponent>("RightHandCollisionComponent");
	RightHandCollisionComponent->SetupAttachment(RootComponent);
	
}

// Called when the game starts or when spawned
void AZombieBase::BeginPlay()
{
	Super::BeginPlay();

	uint8 RandomMesh = FMath::RandRange(0, ZombieMeshes.Num() - 1);
	/*GetMesh()->SetSkeletalMesh(ZombieMeshes[RandomMesh->SkeletalMesh]);*/
	GetMesh()->SetSkeletalMesh(ZombieMeshes[RandomMesh]);
	//for (USkeletalMesh* ZombieMesh : ZombieMeshes)
	//{
	//	if (ZombieMesh != GetMesh()->SkeletalMesh)
	//	{
	//		// Change to actually delete the meesh to improve performance
	//		ZombieMeshes.Remove(ZombieMesh);
	//	}
	//}
	//ZombieMeshes.Empty();
	

	UCharacterMovementComponent* MovementPtr = Cast<UCharacterMovementComponent>(this->GetCharacterMovement());
	uint8 RandomSpeed = FMath::RandRange(0, 2);
	MovementPtr->MaxWalkSpeed = MovementSpeeds[RandomSpeed];

	const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);

	LeftHandCollisionComponent->AttachToComponent(GetMesh(), AttachmentRules, "fist_l_collision");
	RightHandCollisionComponent->AttachToComponent(GetMesh(), AttachmentRules, "fist_r_collision");

	LeftHandCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AZombieBase::OnOverlapBegin);
	RightHandCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AZombieBase::OnOverlapBegin);

	/*LeftHandCollisionComponent->OnComponentEndOverlap.AddDynamic(this, &AZombieBase::OnOverlapEnd);
	RightHandCollisionComponent->OnComponentEndOverlap.AddDynamic(this, &AZombieBase::OnOverlapEnd);*/

	AudioComponent = UGameplayStatics::SpawnSoundAttached(SlowZombieSound, GetMesh(), TEXT("soundSocket"));
}

void AZombieBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ACharacterBase* Player = Cast<ACharacterBase>(OtherActor))
	{
		if (bCanDoDamage)
		{
			bCanDoDamage = false;
			bInRangeToAttack = true;
			GetWorld()->GetTimerManager().SetTimer(DamageTimerHandle, this, &AZombieBase::SetCanDoDamage, 1.2f, false);
			Player->DecrementHealth(10);
		}	
	}
}

void AZombieBase::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ACharacterBase* Player = Cast<ACharacterBase>(OtherActor))
	{
		bCanDoDamage = true;
		bInRangeToAttack = false;
		/*UE_LOG(LogTemp, Warning, TEXT("OVERLAP END WITH: %s"), *OtherActor->GetName());*/
	}
}




void AZombieBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AZombieBase, bIsDead);
}


void AZombieBase::DecrementHealth(float Damage)
{
	/*UE_LOG(LogTemp, Warning, TEXT("Weapon Damage: %f"), Damage);*/
	if (HasAuthority())
	{
		Health -= Damage;
		/*UE_LOG(LogTemp, Warning, TEXT("Health Left: %f"), Health);*/
		if (Health <= 0)
		{
			AEscapeGameMode* GameMode = GetWorld()->GetAuthGameMode<AEscapeGameMode>();
			if (GameMode != nullptr)
			{
				GameMode->DecrementZombiesOnMapCount();
			}
			bIsDead = true;
			Die();	
		}
	}
}

void AZombieBase::SetCanDoDamage()
{
	bInRangeToAttack = false;
	//UE_LOG(LogTemp, Warning, TEXT("IN RANGE TO ATTACK %s"), bInRangeToAttack ? TEXT("true") : TEXT("false"));
	bCanDoDamage = true;
}

void AZombieBase::OnRep_Die()
{
	
}

void AZombieBase::OnCleanup()
{
	
	Destroy();
}

void AZombieBase::Die()
{
	bIsDead = true;
	if (AudioComponent != nullptr)
	{
		if (AudioComponent->IsActive())
		{
			AudioComponent->Stop();
			AudioComponent->DestroyComponent();
		}
	}
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftHandCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightHandCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetSimulatePhysics(true);
	if (HasAuthority())
	{
		if (AController* AIController = GetController<AController>())
		{
			AIController->Destroy();
		}
		FTimerHandle TempHandle;
		GetWorld()->GetTimerManager().SetTimer(TempHandle, this, &AZombieBase::OnCleanup, CleanupDelay, false);
	}
}



uint8 AZombieBase::GetHitPart(FString BoneName)
{
	if (BoneName.Contains(FString("ead")))
	{ // head hit
	return 5;
	}
	else if (BoneName.Contains(FString("l")) || BoneName.Contains(FString("r")))
	{ // limb hit
		return 1;
	}
	else if (BoneName.Equals(FString("spine_02")) || BoneName.Equals(FString("spine_03")))
	{ // chest hit
		return 2;
	}
	else if (BoneName.Equals(FString("spine_01")) || BoneName.Contains(FString("pelvis")))
	{ // abdomen hit
		return 3;
	}
	else if (BoneName.Equals(FString("neck_01")))
	{ // neck hit
		return 4;
	}
	
	return 0;
}

void AZombieBase::Hit(ACharacterBase* Player, AKnife* Knife)
{
	if (HasAuthority() && Player)
	{	
		if (Knife)
		{
			DecrementHealth(Knife->GetKnifeDamage());
		}
	}
}

void AZombieBase::Hit(ACharacterBase* Player, FHitResult HitResult, float PenetrationDamageMultiplier)
{
	if (HasAuthority() && Player && !bIsDead)
	{
		FString BoneName = HitResult.BoneName.ToString();
		if (BoneName == FString("None")) { return; }
		if (uint8 HitPart = GetHitPart(BoneName))
		{
			// UE_LOG(LogTemp, Warning, TEXT("BONE NAME: %s"), *BoneName);
			if (AWeaponBase* PlayerWeapon = Player->GetCurrentWeapon())
			{
				EHitLocation HitLocation = EHitLocation::None;
				float WeaponDamage = PlayerWeapon->GetWeaponDamage().GetDamage(HitLocation);
				WeaponDamage *= PenetrationDamageMultiplier;
							
				switch(HitPart)
				{
				case 1: {DecrementHealth(WeaponDamage); return; }	// limbs
				case 2: {DecrementHealth(WeaponDamage * 1.5); return;  } // chest
				case 3: {DecrementHealth(WeaponDamage * 1.3); return; } // abdomen
				case 4: {DecrementHealth(WeaponDamage * 1.5); return; } // neck
				case 5: {DecrementHealth(WeaponDamage * 3.0); return; } // head	
				}
			}
		}
	}
}

bool AZombieBase::GetInRangeToAttack()
{
	return bInRangeToAttack;
}

bool AZombieBase::IsDead() const
{
	return bIsDead;
}



