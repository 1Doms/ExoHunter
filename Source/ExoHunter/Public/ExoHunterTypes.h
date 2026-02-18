// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <enet6/enet.h> // On a besoin de connaître ENet ici
#include "ExoHunterTypes.generated.h"

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
	Error			UMETA(DisplayName = "Network Error")
};

USTRUCT()
struct FConnectedPlayer
{
	GENERATED_BODY()

	ENetPeer* Peer = nullptr;
	uint32 InternalID = 0;
	EExoPlayerStatus Status = EExoPlayerStatus::Connecting;

	// TODO FUTUR : Ajouter ici le "Ping" lissé (AverageRTT)
	// uint32 Ping = 0;

	// TODO FUTUR : Ajouter le Timestamp du dernier paquet reçu (pour détecter les Timeouts nous-mêmes si besoin)
	// double LastPacketTime = 0.0f;
};