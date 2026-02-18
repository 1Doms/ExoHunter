// Fill out your copyright notice in the Description page of Project Settings.


#include "UNet6ServerSubsystem.h"

bool UNet6ServerSubsystem::IsAllowedToTick() const
{
    // On ne tick QUE si le serveur est réellement démarré (Host valide)
    return ServerHost != nullptr;
}

bool UNet6ServerSubsystem::StartServer(int32 Port)
{
	// 1. On nettoie au cas où
	StopServer();

	UE_LOG(LogExoServer, Log, TEXT("Démarrage du serveur sur le port %d..."), Port);

	// 2. Configuration de l'adresse d'écoute
	ENetAddress Address;
	// ENET_HOST_ANY signifie "Ecoute sur toutes les cartes réseaux (Wifi, Ethernet, Localhost)"
	Address.type = ENET_ADDRESS_TYPE_ANY;
	Address.port = Port;

	// 3. Création du Host Serveur (Spécifique ENet6)
	// Arg 1 : Type d'adresse (ANY pour accepter IPv4 et IPv6 si possible)
	// Arg 2 : L'adresse de bind
	// Arg 3 : Nombre max de clients (32 ici)
	// Arg 4 : Nombre de channels (2)
	// Arg 5/6 : Bandwidth (0 = illimité)
	ServerHost = enet_host_create(ENET_ADDRESS_TYPE_ANY, &Address, 32, 2, 0, 0);

	if (!ServerHost)
	{
		UE_LOG(LogExoServer, Error, TEXT("Echec critique : Impossible de créer le Host ENet Serveur (Port occupé ?)."));
		return false;
	}

	UE_LOG(LogExoServer, Log, TEXT("Serveur démarré avec succès ! En attente de clients..."));
	return true;
}

void UNet6ServerSubsystem::StopServer()
{
	if (!ServerHost) return;

	UE_LOG(LogExoServer, Log, TEXT("Arrêt du serveur... Déconnexion des joueurs en cours."));

	// 1. On parcourt TOUS les slots de connexion (peers) alloués par ENet
	for (size_t i = 0; i < ServerHost->peerCount; ++i)
	{
		ENetPeer* CurrentPeer = &ServerHost->peers[i];

		// Si ce peer est actif (Connecté ou en train de se connecter)
		if (CurrentPeer->state != ENET_PEER_STATE_DISCONNECTED)
		{
			// On envoie le signal de déconnexion douce.
			// Le '0' est le code de raison. (Tu pourrais mettre 404 ou ce que tu veux).
			enet_peer_disconnect(CurrentPeer, 0);
		}
	}
	enet_host_flush(ServerHost);


	enet_host_destroy(ServerHost);
	ServerHost = nullptr;

	ConnectedPlayers.Empty();

	UE_LOG(LogExoServer, Log, TEXT("Serveur éteint et mémoire libérée."));
}

void UNet6ServerSubsystem::Tick(float DeltaTime)
{
	if (!ServerHost) return;

	ENetEvent Event;

	// Boucle de traitement des événements (Non-bloquante : timeout = 0)
	while (enet_host_service(ServerHost, &Event, 0) > 0)
	{
		switch (Event.type)
		{
		case ENET_EVENT_TYPE_CONNECT:
		{
			HandleClientConnect(Event.peer);
			break;
		}

		case ENET_EVENT_TYPE_RECEIVE:
		{
			// Note : Event.packet sera détruit automatiquement par notre helper
			HandleReceivePacket(Event.peer, Event.packet);
			break;
		}

		case ENET_EVENT_TYPE_DISCONNECT:
		{
			HandleClientDisconnect(Event.peer);
			break;
		}
		}
	}
}

void UNet6ServerSubsystem::HandleClientConnect(ENetPeer* Peer)
{
	uint32 NewID = NextPlayerID++;

	Peer->data = (void*)(uintptr_t)NewID;

	FConnectedPlayer NewPlayer;
	NewPlayer.Peer = Peer;
	NewPlayer.InternalID = NewID;
	NewPlayer.Status = EExoPlayerStatus::Connecting;

	// Add to map
	ConnectedPlayers.Add(NewID, NewPlayer);

	UE_LOG(LogExoServer, Log, TEXT("Nouveau Client Connecté ! ID ENet : %u"), Peer->connectID);

	if (OnNetworkEvent.IsBound())
	{
		TArray<uint8> EmptyPayload; // Pas de données pour une connexion
		OnNetworkEvent.Broadcast(EExoNetEventType::Connect, NewID, EmptyPayload);
	}
}

void UNet6ServerSubsystem::HandleClientDisconnect(ENetPeer* Peer)
{
	uint32 PlayerID = (uint32)(uintptr_t)Peer->data;

	UE_LOG(LogExoServer, Log, TEXT("Client Déconnecté ID: %u"), PlayerID);

	if (OnNetworkEvent.IsBound())
	{
		TArray<uint8> EmptyPayload;
		OnNetworkEvent.Broadcast(EExoNetEventType::Disconnect, PlayerID, EmptyPayload);
	}

	// On retire Player de la map
	if (ConnectedPlayers.Contains(PlayerID))
	{
		ConnectedPlayers.Remove(PlayerID);
	}

	Peer->data = nullptr;
}

void UNet6ServerSubsystem::HandleReceivePacket(ENetPeer* Peer, const ENetPacket* Packet)
{
	uint32 PlayerID = (uint32)(uintptr_t)Peer->data;

	// Copie des données et est détruit automatiquement grâce au helper
	TArray<uint8> Data = UNet6BaseSubsystem::ConsumePacket((ENetPacket*)Packet);

	if (Data.Num() > 0)
	{
		// BROADCAST : On envoie le paquet au GameMode
		if (OnNetworkEvent.IsBound())
		{
			OnNetworkEvent.Broadcast(EExoNetEventType::DataPacket, PlayerID, Data);
		}
	}
}