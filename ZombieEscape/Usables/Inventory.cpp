// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory.h"
#include "Pickup.h"

// Sets default values for this component's properties
UInventory::UInventory()
{
	/*SetIsReplicated(true);*/
}


// Called when the game starts
void UInventory::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

bool UInventory::AddItem(APickup* Item)
{
	Items.Add(Item);
	return false;
}

void UInventory::RemoveItem(APickup* Item)
{
}

bool UInventory::ItemIsInInventory(APickup* Key) const
{
	if (!Key) { return false; }
	if (Items.Contains(Key))
	{
		return true;
	}
	return false;
}

TArray<class APickup*> UInventory::GetInventoryItems()
{
	return Items;
}

int32 UInventory::GetCurrentInventoryCount()
{
	return Items.Num() - 1;
}


