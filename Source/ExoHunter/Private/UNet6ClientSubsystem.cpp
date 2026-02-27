// Fill out your copyright notice in the Description page of Project Settings.

#include "UNet6ClientSubsystem.h"
#include "ExoHunterNetUtils.h"
#include "ExoHunterOpcodeRouter.h"
#include "enet6/enet.h"

DEFINE_LOG_CATEGORY(LogExoClient);

bool UNet6ClientSubsystem::IsAllowedToTick() const
{
	// Optimisation : On ne tick que si le client ENet est créé (en cours de connexion ou connecté)
	return ClientHost != nullptr;
}

bool UNet6ClientSubsystem::ConnectToServer(FString AddressString, int32 Port, EExoIPVersion IPVType)
{
	// 1. Nettoyage préventif (au cas où on était déjà connecté)
	DisconnectFromServer();

	FString FinalAddress = AddressString;
	if (FinalAddress.Equals(TEXT("localhost"), ESearchCase::IgnoreCase))
	{
		// Si on a explicitement demandé de l'IPv6, le localhost est "::1"
		if (IPVType == EExoIPVersion::IPv6)
		{
			FinalAddress = TEXT("::1");
		}
		else // Pour IPv4 ou Any, on force l'IP locale classique
		{
			FinalAddress = TEXT("127.0.0.1");
		}
	}
	
	UE_LOG(LogExoClient, Log, TEXT("Tentative de connexion à %s:%d..."), *FinalAddress, Port);
	ENetAddressType TargetENetType = static_cast<ENetAddressType>(ExoHunterNetUtils::GetENetAddressType(IPVType));
	// 2. Configuration de l'adresse
	ENetAddress serverAddress;
	if (enet_address_set_host(&serverAddress, TargetENetType, TCHAR_TO_UTF8(*FinalAddress)) != 0)
	{
		UE_LOG(LogExoClient, Error, TEXT("Failed to resolve address"));
	}
	serverAddress.port = Port;
	
	UE_LOG(LogExoClient, Log, TEXT("Connecting..."));

	// 3. Création du Host Client
	// Paramètres : (Type d'adresse, Pas de pairs entrants, 1 Canal, 0 bandwith in/out)
	ClientHost = enet_host_create(serverAddress.type, nullptr, 1, 2, 0, 0);
	if (!ClientHost)
	{
		UE_LOG(LogExoClient, Error, TEXT("Echec critique : Impossible de créer le Host ENet Client."));
		return false;
	}

	// 4. Lancement de la connexion
	ServerPeer = enet_host_connect(ClientHost, &serverAddress, 2, 0);

	if (!ServerPeer)
	{
		UE_LOG(LogExoClient, Error, TEXT("Echec : Impossible d'initier la connexion vers le serveur."));
		return false;
	}

	UE_LOG(LogExoClient, Warning, TEXT("Tentative de connexion lancée en arrière-plan..."));
	return true;
}

void UNet6ClientSubsystem::DisconnectFromServer()
{
	// Si on est connecté, on dit au revoir poliment
	if (ServerPeer)
	{
		enet_peer_disconnect(ServerPeer, 0);
		// On force l'envoi du message de déconnexion
		enet_host_flush(ClientHost);
		ServerPeer = nullptr;
	}

	// On détruit le socket client
	if (ClientHost)
	{
		enet_host_destroy(ClientHost);
		ClientHost = nullptr;
	}

	UE_LOG(LogExoClient, Log, TEXT("Client déconnecté et nettoyé."));
}

void UNet6ClientSubsystem::SendToServer(const FInstancedStruct& PacketData, bool bReliable)
{
	if (!ServerPeer) return; 

	ENetPacket* ENetPacket = FPacketBuilder::BuildPacket(PacketData, bReliable);
	if (ENetPacket)
		InternalSendPacket(ENetPacket);
}

void UNet6ClientSubsystem::Tick(float DeltaTime)
{
	// Sécurité (même si IsAllowedToTick le gère)
	if (!ClientHost) return;

	ENetEvent Event;

	// On traite TOUS les événements en attente (timeout 0 = non bloquant)
	while (enet_host_service(ClientHost, &Event, 0) > 0)
	{
		switch (Event.type)
		{
		case ENET_EVENT_TYPE_CONNECT:
		{	
			BP_OnConnectEvent();
			UE_LOG(LogExoClient, Log, TEXT("Host Service Connected"));
			break;
		}

		case ENET_EVENT_TYPE_DISCONNECT:
		{
			BP_OnDisconnectEvent();
			UE_LOG(LogExoClient, Warning, TEXT("Déconnecté par le serveur."));
			ServerPeer = nullptr; // Le pointeur n'est plus valide
				
			break;
		}
		case ENET_EVENT_TYPE_RECEIVE:
		{
			// On consomme le paquet (Copie + Destruction automatique)
			HandleReceivePacket(Event.packet);
			break;
		}
		}
	}
}

void UNet6ClientSubsystem::HandleReceivePacket(const ENetPacket* Packet)
{
	if (!Packet) return;
	ExoHunterOpcodeRouter::RouteServerMessage(GetWorld(), Packet);
	BP_OnReceivePacketEvent(Packet->dataLength);
	// Nettoyage
	enet_packet_destroy(const_cast<ENetPacket*>(Packet));
}

void UNet6ClientSubsystem::InternalSendPacket(ENetPacket* PacketToSend)
{
	if (ServerPeer && PacketToSend)
		enet_peer_send(ServerPeer, 0, PacketToSend);
}
