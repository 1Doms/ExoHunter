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

	//Server to client
	S_ClientConnecting = 128	 UMETA(DisplayName = "[SV] Client Connecting"),
	S_PlayerJoin				 UMETA(DisplayName = "[SV] Player / Join Success"),
	S_SpawnActor				 UMETA(DisplayName = "[SV] Spawn Network Actor"),
	S_PlayerLeave				 UMETA(DisplayName = "[SV] Player Leave"),
	S_DestroyActor				 UMETA(DisplayName = "[SV] Destroy Network Actor"),
	S_GameEvent					 UMETA(DisplayName = "[SV] Gameplay Event"),
	S_Snapshot					 UMETA(DisplayName = "[SV] World Snapshot"),
	S_MatchState				 UMETA(DisplayName = "[SV] Match State Update"),
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

// --- Player ---
UENUM(BlueprintType)
enum class EExoClassID : uint8
{
	ExoHunter		UMETA(DisplayName = "ExoHunter"),
	Damned 			UMETA(DisplayName = "Damned")
};

UENUM(BlueprintType)
enum class EExoPlayerStatus : uint8
{
	Connecting		UMETA(DisplayName = "Connecting (Handshake)"),
	Loading			UMETA(DisplayName = "Loading Map"),
	Playing			UMETA(DisplayName = "Playing / Active"),
	Spectating		UMETA(DisplayName = "Spectating (Dead)")
};

enum class EExoInputFlags : uint16
{
	None       = 0,
	Forward    = 1 << 0,
	Backward   = 1 << 1,
	Left       = 1 << 2,
	Right      = 1 << 3,
	Space      = 1 << 4,
	LeftShift  = 1 << 5,
	LeftControl= 1 << 6,
	LeftClick  = 1 << 7,
	RightClick = 1 << 8,
	Reload     = 1 << 9,
	Primary = 1 << 10,
	Secondary = 1 << 11,
	Tertiary = 1 << 12,
	quaternary = 1 << 13,
	Ultimate = 1 << 14,
};
ENUM_CLASS_FLAGS(EExoInputFlags);

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
	int32 TotalShieldDamageDealt = 0;

	UPROPERTY(BlueprintReadWrite, Category = "ExoNetwork|Client")
	int32 TotalShieldBreak = 0;

	// --- LE "PAWN" ---
	UPROPERTY(BlueprintReadWrite, Category = "ExoNetwork|Client")
	AActor* ControlledActor = nullptr; 
};

// --- Game Enum ---

UENUM(BlueprintType)
enum class EMatchPhase : uint8
{
	WaitingForPlayers,
	Playing,          
	GameOver          
};

UENUM(BlueprintType)
enum class EGamePhase : uint8
{
	Early,
	Mid,
	Late
};

// --- Settings ---
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

USTRUCT(BlueprintType)
struct FExoMatchSettings
{
	GENERATED_BODY()

	// La durée de la partie en secondes (ex: 600 = 10 minutes)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ExoRules")
	int32 TimeLimitSeconds = 600;

	// Le nombre maximum de joueurs autorisés (pourrait remplacer ta variable en dur !)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ExoRules")
	int32 MaxPlayers = 16;
};

USTRUCT(BlueprintType)
struct FServerGameStateStruct
{
	GENERATED_BODY()

	// Quelle est la phase actuelle du jeu ?
	UPROPERTY(BlueprintReadWrite, Category = "ExoNetwork|GameState")
	EMatchPhase currentMatchPhase = EMatchPhase::WaitingForPlayers;
	
	UPROPERTY(BlueprintReadWrite, Category = "ExoNetwork|GameState")
	EGamePhase currentGamePhase = EGamePhase::Early;

	// Le temps restant en secondes (pour afficher le timer en haut de l'écran)
	UPROPERTY(BlueprintReadWrite, Category = "ExoNetwork|GameState")
	int32 MatchTimeRemaining = 0;

	// Si c'est un jeu en équipe, les scores globaux !
	UPROPERTY(BlueprintReadWrite, Category = "ExoNetwork|GameState")
	int32 ExoHunterTeam = 0;

	UPROPERTY(BlueprintReadWrite, Category = "ExoNetwork|GameState")
	int32 DamnedTeam = 0;
	
	UPROPERTY(BlueprintReadWrite, Category = "ExoNetwork|GameState")
	int32 OverlordTeam = 0;
};