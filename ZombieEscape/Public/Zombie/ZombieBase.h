// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Engine.h"
#include "ZombieBase.generated.h"

UCLASS()
class ZOMBIEESCAPE_API AZombieBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AZombieBase();

protected:
	float Health;
	bool bCanDoDamage;
	TArray<int16> MovementSpeeds;

	FTimerHandle DamageTimerHandle;

	UAudioComponent* AudioComponent = nullptr;

	void SetCanDoDamage();

	UPROPERTY(EditDefaultsOnly)
		float CleanupDelay;

	UPROPERTY(ReplicatedUsing = OnRep_Die, EditAnywhere)
	bool bIsDead;

	UFUNCTION()
	void OnRep_Die();

	void OnCleanup();

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
		USoundBase* SlowZombieSound = nullptr;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
		USoundBase* FastZombieSound = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	uint8 GetHitPart(FString BoneName);
	void DecrementHealth(float Damage);
	void Die();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	void Hit(class ACharacterBase* Player, class AKnife* Knife);
	void Hit(class ACharacterBase* Player, FHitResult HitResult, float PenetrationDamageReduction);

	UPROPERTY(EditDefaultsOnly, Category = "EscapeSettings")
		class UBoxComponent* RightHandCollisionComponent;

	UPROPERTY(EditDefaultsOnly, Category = "EscapeSettings")
		class UBoxComponent* LeftHandCollisionComponent;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
	TArray<USkeletalMesh*> ZombieMeshes;

	bool bInRangeToAttack;

	UFUNCTION(BlueprintCallable)
	bool GetInRangeToAttack();

	UFUNCTION(BlueprintPure)
	bool IsDead() const;

};
