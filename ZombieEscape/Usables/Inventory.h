// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZOMBIEESCAPE_API UInventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventory();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	TArray<class APickup*> Items;

public:	

	bool AddItem(class APickup* Item);
	void RemoveItem(class APickup* Item);
	bool ItemIsInInventory(APickup* Key) const;

	UFUNCTION(BlueprintCallable)
	TArray<class APickup*> GetInventoryItems();

	UFUNCTION(BlueprintCallable)
	int32 GetCurrentInventoryCount();
};
