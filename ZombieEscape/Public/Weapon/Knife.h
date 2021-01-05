// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Knife.generated.h"

UCLASS()
class ZOMBIEESCAPE_API AKnife : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKnife();

protected:
	UPROPERTY(VisibleAnywhere, Category = "EscapeSettings")
	class UStaticMeshComponent* KnifeMesh;

	UPROPERTY(EditDefaultsOnly, Category = "EscapeSettings")
	class UBoxComponent* CollisionComponent;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
	class UAnimMontage* FPSKnifeMontage;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
	UParticleSystem* BodyImpactEffect;

	bool bCanDamage;
	bool bCanAttack;

	UPROPERTY(EditAnywhere, Category = "EscapeSettings")
		int16 KnifeDamage;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

public:	
	void OnKnife();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE class UStaticMeshComponent* GetKnifeMesh() { return KnifeMesh; }

	UFUNCTION(BlueprintCallable)
		void SetCanDamage(bool CanCauseDamage);

	UFUNCTION(BlueprintCallable)
		void SetCanAttack(bool CanAttack);

	FORCEINLINE int16 GetKnifeDamage() { return KnifeDamage; }
};
