// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ExoHunterTypes.h"
#include "PacketBuilder.h"
#include "UNet6BaseSubsystem.h"
#include "UNet6ServerSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogExoServer, Log, All);

/**
 * G�re l'h�bergement de la partie (Host).
 * Re�oit les connexions et les inputs des clients.
 */

struct _ENetHost;
typedef _ENetHost ENetHost;


UCLASS(Abstract, Blueprintable)
class EXOHUNTER_API UNet6ServerSubsystem : public UNet6BaseSubsystem
{
	GENERATED_BODY()

public:
	// --- API Blueprint ---

	UFUNCTION(BlueprintCallable, Category = "ExoNetwork|Server")
	bool StartServer(int32 Port = 7777, EExoIPVersion IPVType = EExoIPVersion::IPv4);

	UFUNCTION(BlueprintCallable, Category = "ExoNetwork|Server")
	void StopServer();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "ExoNetwork|Server")
	void BP_OnServerStartEvent();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "ExoNetwork|Server")
	void BP_OnServerStopEvent();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "ExoNetwork|Server")
	void BP_OnClientConnectEvent(int32 ClientID, EExoPlayerStatus Status);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "ExoNetwork|Server")
	void BP_OnClientDisconnectEvent(int32 ClientID);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "ExoNetwork|Server")
	void BP_OnReceivePacketEvent(int32 ClientID);
	
	// --- ENVOIE AU JOUEUR CONCERNE
	UFUNCTION(BlueprintCallable, Category = "ExoNetwork|Server", meta = (BaseStruct = "ExoServerPacket"))
	void SendToPlayer(int32 PlayerID, const FInstancedStruct& PacketData, bool bReliable = true);

	// --- ENVOI A TOUT LE MONDE ---
	UFUNCTION(BlueprintCallable, Category = "ExoNetwork|Server", meta = (BaseStruct = "ExoServerPacket"))
	void SendToAllPlayers(const FInstancedStruct& PacketData, bool bReliable = true);

protected:
	virtual void Tick(float DeltaTime) override;
	virtual bool IsAllowedToTick() const override;
	virtual void Deinitialize() override;

private:
	// Le socket d'�coute
	ENetHost* ServerHost = nullptr;
	TMap<uint32, FConnectedClient> ConnectedClients;
	uint32 NextClientID = 1;

	// --- Handlers Internes ---
	void HandleClientConnect(ENetPeer* Peer);
	void HandleClientDisconnect(ENetPeer* Peer);
	void HandleReceivePacket(ENetPeer* Peer, const ENetPacket* Packet);
	
	void InternalSendToPlayer(uint32 PlayerID, ENetPacket* PacketToSend);
	void InternalSendToAll(ENetPacket* PacketToSend);
};
