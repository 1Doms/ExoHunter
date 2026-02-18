// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UNet6BaseSubsystem.h"
#include "UNet6ClientSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogExoClient, Log, All);

UCLASS()
class EXOHUNTER_API UNet6ClientSubsystem : public UNet6BaseSubsystem
{
	GENERATED_BODY()

public:
	// --- API Blueprint ---

	UFUNCTION(BlueprintCallable, Category = "ExoNetwork|Client")
	bool Connect(FString Address, int32 Port = 14769);

	UFUNCTION(BlueprintCallable, Category = "ExoNetwork|Client")
	void Disconnect();

	virtual void Tick(float DeltaTime) override;

	virtual bool IsAllowedToTick() const override;

private:
	ENetHost* ClientHost = nullptr; // Client socket
	ENetPeer* ServerPeer = nullptr; // Le serveur auquel on est connecté

	void HandleReceivePacket(const ENetPacket* Packet);
};