// Fill out your copyright notice in the Description page of Project Settings.


#include "UNet6BaseSubsystem.h"
#include <enet6/enet.h>
#include "Tickable.h"

int32 UNet6BaseSubsystem::ENetInitCount = 0;

void UNet6BaseSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (ENetInitCount == 0)
	{
		if (enet_initialize() != 0)
		{
			UE_LOG(LogTemp, Fatal, TEXT("[ENetBase] CRITICAL: Failed to initialize ENet Library!"));
			return;
		}
		UE_LOG(LogTemp, Log, TEXT("[ENetBase] ENet Library Initialized successfully."));
	}
// On incr�mente le compteur (ex: 1 pour le Server, puis 2 quand le Client arrive)
	ENetInitCount++;
}

void UNet6BaseSubsystem::Deinitialize()
{
	// On d�cr�mente le compteur quand un subsystem est d�truit
	ENetInitCount--;

	// Si le compteur retombe � 0 (ou moins par s�curit�), plus personne n'utilise ENet.
	// On peut donc �teindre la librairie proprement.
	if (ENetInitCount <= 0)
	{
		enet_deinitialize();
		UE_LOG(LogTemp, Log, TEXT("[ENetBase] ENet Library Deinitialized."));

		// Reset propre � 0
		ENetInitCount = 0;
	}

	Super::Deinitialize();
}

void UNet6BaseSubsystem::Tick(float DeltaTime)
{
}