// Fill out your copyright notice in the Description page of Project Settings.


#include "UNet6ClientSubsystem.h"

DEFINE_LOG_CATEGORY(LogExoClient);

bool UNet6ClientSubsystem::IsAllowedToTick() const
{
	// Optimisation : On ne tick que si le client ENet est créé (en cours de connexion ou connecté)
	return ClientHost != nullptr;
}

bool UNet6ClientSubsystem::Connect(FString AddressString, int32 Port)
{
	// 1. Nettoyage préventif (au cas où on était déjà connecté)
	Disconnect();

	UE_LOG(LogExoClient, Log, TEXT("Tentative de connexion à %s:%d..."), *AddressString, Port);

	// 2. Configuration de l'adresse
	ENetAddress Address;
	enet_address_set_host(&Address, ENET_ADDRESS_TYPE_ANY, TCHAR_TO_UTF8(*AddressString));
	Address.port = Port;

	// 3. Création du Host Client
	// Paramètres : (Type d'adresse, Pas de pairs entrants, 1 Canal, 0 bandwith in/out)
	ClientHost = enet_host_create(Address.type, nullptr, 1, 2, 0, 0);

	if (!ClientHost)
	{
		UE_LOG(LogExoClient, Error, TEXT("Echec critique : Impossible de créer le Host ENet Client."));
		return false;
	}

	// 4. Lancement de la connexion
	ServerPeer = enet_host_connect(ClientHost, &Address, 2, 0);

	if (!ServerPeer)
	{
		UE_LOG(LogExoClient, Error, TEXT("Echec : Impossible d'initier la connexion vers le serveur."));
		return false;
	}

	// 5. Attente synchrone (5 secondes max) pour valider la connexion
	// C'est le seul moment "bloquant", nécessaire pour savoir si on change de level ou pas.
	ENetEvent Event;

	// On tente pendant 50 tours de 100ms = 5 secondes
	for (int i = 0; i < 50; i++)
	{
		if (enet_host_service(ClientHost, &Event, 100) > 0)
		{
			if (Event.type == ENET_EVENT_TYPE_CONNECT)
			{
				UE_LOG(LogExoClient, Log, TEXT("Connexion REUSSIE au serveur !"));
				return true;
			}
		}
	}

	// 6. Si on arrive ici, c'est un échec (Timeout)
	UE_LOG(LogExoClient, Error, TEXT("Timeout : Le serveur ne répond pas."));
	Disconnect(); // Nettoyage
	return false;
}

void UNet6ClientSubsystem::Disconnect()
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
			UE_LOG(LogExoClient, Log, TEXT("Host Service Connected"));
			break;
		}

		case ENET_EVENT_TYPE_DISCONNECT:
		{
			UE_LOG(LogExoClient, Warning, TEXT("Déconnecté par le serveur."));
			ServerPeer = nullptr; // Le pointeur n'est plus valide
			break;
		}
		case ENET_EVENT_TYPE_RECEIVE:
		{
			// 1. On consomme le paquet (Copie + Destruction automatique)
			HandleReceivePacket(Event.packet);
			break;
		}
		}
	}
}

void UNet6ClientSubsystem::HandleReceivePacket(const ENetPacket* Packet)
{
	// 1. Conversion propre ENet -> Unreal (et destruction du packet ENet)
	TArray<uint8> Data = UNet6BaseSubsystem::ConsumePacket((ENetPacket*)Packet);

	// 2. Vérification de base
	if (Data.Num() == 0) return;

	UE_LOG(LogExoClient, Verbose, TEXT("Paquet reçu : %d octets"), Data.Num());

	// TODO:
	// 1. Lire Opcode (Data[0])
	// 2. Switch(Opcode)
	// 3. SerializationLib::ReadStruct(...)
}