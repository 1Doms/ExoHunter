// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ExoHunterTypes.generated.h"

struct _ENetPeer;
typedef _ENetPeer ENetPeer;

// 1. Les Opcodes visibles en Blueprint
UENUM(BlueprintType)
enum class EExoHunterOpcode : uint8
{
	None = 0  					UMETA(DisplayName = "None"),
	
	//Client to Server
	C_Connect = 1	    		 UMETA(DisplayName = "Client Connect"),
	C_Disconnect       			 UMETA(DisplayName = "Client Disconnect"),
	C_Input            			 UMETA(DisplayName = "Client Input"),
	C_Test						UMETA(DisplayName = "Client Test"),

	//Server to client
	S_ClientConnecting = 128	 UMETA(DisplayName = "Client Connecting"),
	S_PlayerJoin				 UMETA(DisplayName = "[SV] Player / Join Success"),
	S_SpawnActor				 UMETA(DisplayName = "[SV] Spawn Network Actor"),
	S_PlayerLeave				 UMETA(DisplayName = "[SV] Player Leave"),
	S_DestroyActor				 UMETA(DisplayName = "[SV] Destroy Network Actor"),
	S_GameEvent					 UMETA(DisplayName = "[SV] Gameplay Event"),
	S_Snapshot					 UMETA(DisplayName = "[SV] World Snapshot"),
	S_MatchState				 UMETA(DisplayName = "[SV] Match State Update"),
	S_Test						UMETA(DisplayName = "[SV] Test Update")
};

UENUM(BlueprintType)
enum class EExoPlayerStatus : uint8
{
	Connecting		UMETA(DisplayName = "Connecting (Handshake)"),
	Loading			UMETA(DisplayName = "Loading Map"),
	Playing			UMETA(DisplayName = "Playing / Active"),
	Spectating		UMETA(DisplayName = "Spectating (Dead)")
};

UENUM(BlueprintType)
enum class EExoNetEventType : uint8
{
	Connect			UMETA(DisplayName = "Client Connected"),
	Disconnect		UMETA(DisplayName = "Client Disconnected"),
	DataPacket		UMETA(DisplayName = "Data Received"),
	Error			UMETA(DisplayName = "Network Error"),
	Timeout			UMETA(DisplayName = "Network Timeout"),
	Kicked			UMETA(DisplayName = "Client Kicked"),
	PingWarning		UMETA(DisplayName = "Ping Warning")
};

UENUM(BlueprintType)
enum class EExoIPVersion : uint8
{
	IPv4	UMETA(DisplayName = "IPv4 (Recommandé en local)"),
	IPv6	UMETA(DisplayName = "IPv6"),
	Any		UMETA(DisplayName = "IPv4 ou IPv6 (Automatique)")
};

UENUM(BlueprintType)
enum class EExoClassID : uint8
{
	ExoHunter		UMETA(DisplayName = "ExoHunter"),
	Damned 			UMETA(DisplayName = "Damned")
};

USTRUCT(BlueprintType)
struct FConnectedClient
{
	GENERATED_BODY()

	ENetPeer* Peer = nullptr;

	// --- LES INFOS RÉSEAU ---
	UPROPERTY(BlueprintReadOnly, Category = "ExoNetwork|Client")
	int32 InternalID = 0;

	UPROPERTY(BlueprintReadWrite, Category = "ExoNetwork|Client")
	EExoPlayerStatus Status = EExoPlayerStatus::Connecting;

	UPROPERTY(BlueprintReadWrite, Category = "ExoNetwork|Client")
	int32 Ping = 0;

	// --- LE "PLAYER STATE" (Les Stats) ---
	UPROPERTY(BlueprintReadWrite, Category = "ExoNetwork|Client")
	FString PlayerName = "Unknown";

	UPROPERTY(BlueprintReadWrite, Category = "ExoNetwork|Client")
	int32 TotalDamageDealt = 0;

	UPROPERTY(BlueprintReadWrite, Category = "ExoNetwork|Client")
	int32 TotalShiedBreak = 0;

	// --- LE "PAWN" ---
	UPROPERTY(BlueprintReadWrite, Category = "ExoNetwork|Client")
	AActor* ControlledActor = nullptr; 
};

USTRUCT(BlueprintType)
struct FServerInfo
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, Category = "ExoNetwork|Server")
	int32 MaxPeerCount = 0;
	
	UPROPERTY(BlueprintReadOnly, Category = "ExoNetwork|Server")
	int32 ConnectedPeers = 0;
	
	UPROPERTY(BlueprintReadOnly, Category = "ExoNetwork|Server")
	TMap<int32, FConnectedClient> ConnectedClients;
};
