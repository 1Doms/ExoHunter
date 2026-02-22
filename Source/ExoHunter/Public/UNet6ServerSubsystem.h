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


UCLASS()
class EXOHUNTER_API UNet6ServerSubsystem : public UNet6BaseSubsystem
{
	GENERATED_BODY()

public:
	// --- API Blueprint ---

	UFUNCTION(BlueprintCallable, Category = "ExoNetwork|Server")
	bool StartServer(int32 Port = 7777, EExoIPVersion IPVType = EExoIPVersion::IPv4);

	UFUNCTION(BlueprintCallable, Category = "ExoNetwork|Server")
	void StopServer();
	
	// --- ENVOIE AU JOUEUR CONCERNE
	template<typename TPacketStruct>
	void SendToPlayer(uint32 PlayerID, const TPacketStruct& Packet, bool bReliable = true)
	{
		if (!ServerHost) return;
		
		ENetPacket* ENetPacket = FPacketBuilder::BuildPacket(Packet, bReliable);
		InternalSendToPlayer(PlayerID, ENetPacket);
	}

	// --- ENVOI A TOUT LE MONDE ---
	template<typename TPacketStruct>
	void SendToAllPlayers(const TPacketStruct& Packet, bool bReliable = true)
	{
		if (!ServerHost) return;

		ENetPacket* ENetPacket = FPacketBuilder::BuildPacket(Packet, bReliable);
		InternalSendToAll(ENetPacket);
	}

protected:
	virtual void Tick(float DeltaTime) override;
	virtual bool IsAllowedToTick() const override;
	virtual void Deinitialize() override;

private:
	// Le socket d'�coute
	ENetHost* ServerHost = nullptr;
	TMap<uint32, FConnectedPlayer> ConnectedPlayers;
	uint32 NextPlayerID = 1;

	// --- Handlers Internes ---
	void HandleClientConnect(ENetPeer* Peer);
	void HandleClientDisconnect(ENetPeer* Peer);
	void HandleReceivePacket(ENetPeer* Peer, const ENetPacket* Packet);
	
	void InternalSendToPlayer(uint32 PlayerID, ENetPacket* PacketToSend);
	void InternalSendToAll(ENetPacket* PacketToSend);
};
