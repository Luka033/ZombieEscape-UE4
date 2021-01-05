// Copyright Luka Jozic 2020

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "InteractableBase.h"
#include "Interacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractChanged, const FString&, OnInteractChanged);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ZOMBIEESCAPE_API UInteracter : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInteracter();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	bool IsCarryingItem() { return CarryingItem; }

protected:

	// Called when the game starts
	virtual void BeginPlay() override;
	FTimerHandle TInteractTimerHandle;
	class AInteractableBase* Interactable;
	void SetInteractableObject();
	void Interact();

	UPROPERTY(BlueprintAssignable)
	FInteractChanged OnInteractChanged;

private:
	UPROPERTY(EditDefaultsOnly)
	float InteractionRange;

	UPROPERTY()
	UPhysicsHandleComponent* PhysicsHandle = nullptr;
	UPROPERTY()
	UInputComponent* InputComponent = nullptr;

	bool CarryingItem = false;

	void Grab();
	void Release();
	void FindPhysicsHandle();
	void SetupInputComponent();

	// Return the first actor within reach with physicsbody
	FHitResult GetFirstPhysicsBodyInReach() const;

	// Return the LineTraceEnd
	FVector GetPlayersReach() const;

	// Get players position in world
	FVector GetPlayersWorldPos() const;


};
