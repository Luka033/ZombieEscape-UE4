// Fill out your copyright notice in the Description page of Project Settings.


#include "WinTriggerBox.h"
#include "..\..\Public\Player\CharacterBase.h"
#include "..\..\Public\Game\EscapeGameMode.h"
#include "DrawDebugHelpers.h"

AWinTriggerBox::AWinTriggerBox()
{
	OnActorBeginOverlap.AddDynamic(this, &AWinTriggerBox::OnOverlapBegin);
}

void AWinTriggerBox::BeginPlay()
{
	Super::BeginPlay();

	/*DrawDebugBox(GetWorld(), GetActorLocation(), GetComponentsBoundingBox().GetExtent(), FColor::Green, true, -1, 0, 5);*/
}

void AWinTriggerBox::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
	if (ACharacterBase* Player = Cast<ACharacterBase>(OtherActor))
	{
		APlayerController* PlayerController = Cast<APlayerController>(Player->GetController());
		if (PlayerController != nullptr)
		{
			PlayerController->GameHasEnded(Player, true);
		}
	}
}
