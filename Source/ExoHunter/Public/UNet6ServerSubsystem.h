// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UNet6BaseSubsystem.h"
#include "ExoHunterTypes.h"
#include "UNet6ServerSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogExoServer, Log, All);

/**
 * Gère l'hébergement de la partie (Host).
 * Reçoit les connexions et les inputs des clients.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnExoNetUniversalEvent, EExoNetEventType, EventType, int32, PlayerId, const TArray<uint8>&, Payload);

UCLASS()
class EXOHUNTER_API UNet6ServerSubsystem : public UNet6BaseSubsystem
{
	GENERATED_BODY()

public:
	// --- API Blueprint ---

	UFUNCTION(BlueprintCallable, Category = "ExoNetwork|Server")
	bool StartServer(int32 Port = 14769);

	UFUNCTION(BlueprintCallable, Category = "ExoNetwork|Server")
	void StopServer();

	UPROPERTY(BlueprintAssignable, Category = "ExoNetwork|Events")
	FOnExoNetUniversalEvent OnNetworkEvent;

protected:
	virtual void Tick(float DeltaTime) override;
	virtual bool IsAllowedToTick() const override;

private:
	// Le socket d'écoute
	ENetHost* ServerHost = nullptr;
	TMap<uint32, FConnectedPlayer> ConnectedPlayers;
	uint32 NextPlayerID = 1;

	// --- Handlers Internes ---
	void HandleClientConnect(ENetPeer* Peer);
	void HandleClientDisconnect(ENetPeer* Peer);
	void HandleReceivePacket(ENetPeer* Peer, const ENetPacket* Packet);
};
