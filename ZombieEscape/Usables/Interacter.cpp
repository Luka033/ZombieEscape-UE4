// Copyright Luka Jozic 2020

#include "Interacter.h"
#include "CharacterBase.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"

#define OUT

// Sets default values for this component's properties
UInteracter::UInteracter()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	Interactable = nullptr;
	InteractionRange = 230.f;
}

// Called when the game starts
void UInteracter::BeginPlay()
{
	Super::BeginPlay();

	FindPhysicsHandle();
	SetupInputComponent();

	GetWorld()->GetTimerManager().SetTimer(TInteractTimerHandle, this, &UInteracter::SetInteractableObject, 0.1f, true);
}

void UInteracter::SetInteractableObject()
{
	FHitResult HitResult;
	FCollisionObjectQueryParams CollisionQuery;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwner());

	GetWorld()->LineTraceSingleByObjectType(
		OUT HitResult,
		GetPlayersWorldPos(),
		GetPlayersReach(),
		CollisionQuery,
		CollisionParams
	);

	AInteractableBase* Temp = Cast<AInteractableBase>(HitResult.GetActor());

	if (Interactable == nullptr && Temp) 
	{
		/*UE_LOG(LogTemp, Warning, TEXT("IS NOW A VALID PTR"));*/

		Interactable = Temp;
		OnInteractChanged.Broadcast(Interactable->GetUIMessage());
	}
	else if (Interactable && Temp == nullptr)
	{
		/*UE_LOG(LogTemp, Warning, TEXT("Is Now A NULL PTR"));*/
		Interactable = nullptr;
		OnInteractChanged.Broadcast(FString());
	}
}

void UInteracter::SetupInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent)
	{
		InputComponent->BindAction("Grab", IE_Pressed, this, &UInteracter::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UInteracter::Release);
		InputComponent->BindAction("Interact", IE_Released, this, &UInteracter::Interact);
	}
}

void UInteracter::Interact()
{
	if (Interactable)
	{
		ACharacterBase* Player = Cast<ACharacterBase>(GetOwner());
		if(Interactable != nullptr)
			Interactable->Use(Player);
	}
}



void UInteracter::FindPhysicsHandle()
{
	// Checking for Physics Handle Component
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (!PhysicsHandle)
	{
		UE_LOG(LogTemp, Error, TEXT("No Physics Handle component found on %s "), *GetOwner()->GetName());
	}
}


void UInteracter::Grab()
{
	FHitResult HitResult = GetFirstPhysicsBodyInReach();	
	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();
	AActor* ActorHit = HitResult.GetActor();

	// if we hit something, attach the physics handle.
	if (ActorHit)
	{
		if (!PhysicsHandle) { return; }
		PhysicsHandle->GrabComponentAtLocation(ComponentToGrab, NAME_None, GetPlayersReach());
		ACharacterBase* Player = Cast<ACharacterBase>(GetOwner());
		UCharacterMovementComponent* MovementComp = Cast<UCharacterMovementComponent>(Player->GetCharacterMovement());
		MovementComp->MaxWalkSpeed = 250.0f; 
		CarryingItem = true;
	}
}


void UInteracter::Release()
{
	if (!PhysicsHandle) { return; }
	PhysicsHandle->ReleaseComponent();
	ACharacterBase* Player = Cast<ACharacterBase>(GetOwner());
	UCharacterMovementComponent* MovementComp = Cast<UCharacterMovementComponent>(Player->GetCharacterMovement());
	MovementComp->MaxWalkSpeed = 600.0f;
	CarryingItem = false;
}


// Called every frame
void UInteracter::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//If the physics handle is attached
	if (!PhysicsHandle) { return; }
	if (PhysicsHandle->GrabbedComponent)
	{
		// Move the object we are holding
		PhysicsHandle->SetTargetLocation(GetPlayersReach());
	}
}



FHitResult UInteracter::GetFirstPhysicsBodyInReach() const
{
	FHitResult Hit;
	// Ray-cast out to a certain distance (Reach)
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		GetPlayersWorldPos(),
		GetPlayersReach(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParams
	);
	return Hit;
}

FVector UInteracter::GetPlayersWorldPos() const
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation);

	return PlayerViewPointLocation;
}

FVector UInteracter::GetPlayersReach() const
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation);

	return PlayerViewPointLocation + PlayerViewPointRotation.Vector() * InteractionRange;
}


