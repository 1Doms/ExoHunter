// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ExoHunterTypes.h"
#include "PacketBuilder.h"
#include "UNet6BaseSubsystem.h"
#include "UNet6ClientSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogExoClient, Log, All);

struct _ENetHost;
typedef _ENetHost ENetHost;

struct _ENetPeer;
typedef _ENetPeer ENetPeer;

struct _ENetPacket;
typedef _ENetPacket ENetPacket;

UCLASS()
class EXOHUNTER_API UNet6ClientSubsystem : public UNet6BaseSubsystem
{
	GENERATED_BODY()
	
public:
	// --- API Blueprint ---

	UFUNCTION(BlueprintCallable, Category = "ExoNetwork|Client")
	bool ConnectToServer(FString IPAddress = TEXT("localhost"), int32 Port = 7777, EExoIPVersion IPVType = EExoIPVersion::IPv4);

	UFUNCTION(BlueprintCallable, Category = "ExoNetwork|Client")
	void DisconnectFromServer();

	template<typename TPacketStruct>
	void SendToServer(const TPacketStruct& Packet, bool bReliable = true)
	{
		if (!ServerPeer) return; // Sécurité : on n'envoie rien si on n'est pas connecté

		// 1. On construit le paquet
		ENetPacket* ENetPacket = FPacketBuilder::BuildPacket(Packet, bReliable);
		InternalSendPacket(ENetPacket);
	}

protected:
	virtual void Tick(float DeltaTime) override;
	virtual bool IsAllowedToTick() const override;

private:
	ENetHost* ClientHost = nullptr; // Client socket
	ENetPeer* ServerPeer = nullptr; // Le serveur auquel on est connect�

	void HandleReceivePacket(const ENetPacket* Packet);
	void InternalSendPacket(ENetPacket* PacketToSend);
};