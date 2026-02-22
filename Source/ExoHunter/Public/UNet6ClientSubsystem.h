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

UCLASS(Abstract, Blueprintable)
class EXOHUNTER_API UNet6ClientSubsystem : public UNet6BaseSubsystem
{
	GENERATED_BODY()
	
public:
	// --- API Blueprint ---

	UFUNCTION(BlueprintCallable, Category = "ExoNetwork|Client")
	bool ConnectToServer(FString IPAddress = TEXT("localhost"), int32 Port = 7777, EExoIPVersion IPVType = EExoIPVersion::IPv4);

	UFUNCTION(BlueprintCallable, Category = "ExoNetwork|Client")
	void DisconnectFromServer();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "ExoNetwork|Client")
	void BP_OnConnectEvent();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "ExoNetwork|Client")
	void BP_OnDisconnectEvent();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "ExoNetwork|Server")
	void BP_OnReceivePacketEvent();
	
	UFUNCTION(BlueprintCallable, Category = "ExoNetwork|Client", meta = (BaseStruct = "ExoClientPacket"))
	void SendToServer(const FInstancedStruct& PacketData, bool bReliable = true);

protected:
	virtual void Tick(float DeltaTime) override;
	virtual bool IsAllowedToTick() const override;

private:
	ENetHost* ClientHost = nullptr; // Client socket
	ENetPeer* ServerPeer = nullptr; // Le serveur auquel on est connect�

	void HandleReceivePacket(const ENetPacket* Packet);
	void InternalSendPacket(ENetPacket* PacketToSend);
};