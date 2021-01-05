// Fill out your copyright notice in the Description page of Project Settings.


#include "Knife.h"
#include "CharacterBase.h"
#include "..\..\Public\Player\CharacterBase.h"
#include "..\..\Public\Zombie\ZombieBase.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"


// Sets default values
AKnife::AKnife()
{
	KnifeMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	RootComponent = KnifeMesh;
	
	CollisionComponent = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	CollisionComponent->SetupAttachment(KnifeMesh);
	CollisionComponent->SetRelativeLocation(FVector(0.0f, 21.0f, 0.0f));
	CollisionComponent->InitBoxExtent(FVector(1.2f, 13.0f, 2.7f));

	bCanDamage = false;
	bCanAttack = true;
	KnifeDamage = 100;
}

// Called when the game starts or when spawned
void AKnife::BeginPlay()
{
	Super::BeginPlay();
	// hide knife by default
	KnifeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AKnife::OnOverlapBegin);
}

void AKnife::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bCanDamage)
	{
		if (AZombieBase* Zombie = Cast<AZombieBase>(OtherActor))
		{
			UE_LOG(LogTemp, Warning, TEXT("Hitting Zombie With Knife: %s"), *OtherActor->GetName());

			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BodyImpactEffect,
				KnifeMesh->GetComponentLocation(),
				KnifeMesh->GetComponentRotation());

			Zombie->Hit(Cast<ACharacterBase>(GetOwner()), this);
			bCanDamage = false;
		}	
	}
}

void AKnife::OnKnife()
{
	if (bCanAttack && FPSKnifeMontage)
	{
		if (ACharacterBase* Player = Cast<ACharacterBase>(GetOwner()))
		{
			if (Player->IsPerformingAction()) { return; }
			if (UAnimInstance* AnimInstance = Player->GetMesh1P()->GetAnimInstance())
			{
				AnimInstance->Montage_Play(FPSKnifeMontage);
				uint8 AnimToPlay = FMath::RandRange(1, 2);
				if(AnimToPlay > 1)
					AnimInstance->Montage_JumpToSection(FName("KnifeAttack2"), FPSKnifeMontage);
				else
					AnimInstance->Montage_JumpToSection(FName("KnifeAttack1"), FPSKnifeMontage);
			}
			Player->SetPerformingAction(true);
		}
	}
}

void AKnife::SetCanDamage(bool CanCauseDamage)
{
	bCanDamage = CanCauseDamage;
}

void AKnife::SetCanAttack(bool CanAttack)
{
	bCanAttack = CanAttack;
}



