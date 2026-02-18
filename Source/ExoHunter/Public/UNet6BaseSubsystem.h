// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include <enet6/enet.h> 
#include "UNet6BaseSubsystem.generated.h"

/**
 * Classe parente abstraite pour gérer l'initialisation d'ENet
 * et les fonctions utilitaires communes.
 */
UCLASS(Abstract)
class EXOHUNTER_API UNet6BaseSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	// --- Cycle de vie du Subsystem ---
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// --- FTickableGameObject Interface (Boilerplate) ---
	virtual void Tick(float DeltaTime) override;
	virtual ETickableTickType GetTickableTickType() const override { return ETickableTickType::Always; }
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UENet6BaseSubsystem, STATGROUP_Tickables); }
	virtual bool IsAllowedToTick() const override { return false; }; // Par défaut, on ne tick pas

	static TArray<uint8> ConsumePacket(ENetPacket* Packet);

protected:
	static int32 ENetInitCount;
};