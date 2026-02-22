// Fill out your copyright notice in the Description page of Project Settings.

#include "UNet6ServerSubsystem.h"
#include "enet6/enet.h"
#include "ExoHunterOpcodeRouter.h"
#include "ExoHunterNetUtils.h"

DEFINE_LOG_CATEGORY(LogExoServer);

bool UNet6ServerSubsystem::IsAllowedToTick() const
{
    // On ne tick QUE si le serveur est r�ellement d�marr� (Host valide)
    return ServerHost != nullptr;
}

bool UNet6ServerSubsystem::StartServer(int32 Port, EExoIPVersion IPVType)
{
	// On nettoie au cas o�
	StopServer();

	UE_LOG(LogExoServer, Log, TEXT("D�marrage du serveur sur le port %d..."), Port);
	
	ENetAddressType TargetENetType = static_cast<ENetAddressType>(ExoHunterNetUtils::GetENetAddressType(IPVType));

	ENetAddress Address;
	enet_address_build_any(&Address, TargetENetType);
	Address.port = Port;
	
	// Cr�ation du Host Serveur (Sp�cifique ENet6)
	// Arg 1 : Type d'adresse (ANY pour accepter IPv4 et IPv6 si possible)
	// Arg 2 : L'adresse de bind
	// Arg 3 : Nombre max de clients (32 ici)
	// Arg 4 : Nombre de channels (2)
	// Arg 5/6 : Bandwidth (0 = illimit�)
	ServerHost = enet_host_create(Address.type, &Address, 32, 2, 0, 0);

	if (!ServerHost)
	{
		UE_LOG(LogExoServer, Error, TEXT("Echec critique : Impossible de cr�er le Host ENet Serveur (Port occup� ?)."));
		return false;
	}

	UE_LOG(LogExoServer, Log, TEXT("Serveur d�marr� avec succ�s ! En attente de clients..."));
	BP_OnServerStartEvent();
	return true;
}

void UNet6ServerSubsystem::StopServer()
{
	if (!ServerHost) return;

	UE_LOG(LogExoServer, Log, TEXT("Arr�t du serveur... D�connexion des joueurs en cours."));

	// On parcourt TOUS les slots de connexion (peers) allou�s par ENet
	for (size_t i = 0; i < ServerHost->peerCount; ++i)
	{
		ENetPeer* CurrentPeer = &ServerHost->peers[i];

		// Si ce peer est actif (Connect� ou en train de se connecter)
		if (CurrentPeer->state != ENET_PEER_STATE_DISCONNECTED)
		{
			// On envoie le signal de d�connexion douce.
			enet_peer_disconnect(CurrentPeer, 0);
		}
	}
	enet_host_flush(ServerHost);


	enet_host_destroy(ServerHost);
	ServerHost = nullptr;

	ConnectedClients.Empty();
	UE_LOG(LogExoServer, Log, TEXT("Serveur �teint et m�moire lib�r�e."));
	
	BP_OnServerStopEvent();
}

void UNet6ServerSubsystem::SendToPlayer(int32 PlayerID, const FInstancedStruct& PacketData, bool bReliable)
{
	if (!ServerHost) return;
		
	ENetPacket* ENetPacket = FPacketBuilder::BuildPacket(PacketData, bReliable);
	InternalSendToPlayer(PlayerID, ENetPacket);
}

void UNet6ServerSubsystem::SendToAllPlayers(const FInstancedStruct& PacketData, bool bReliable)
{
	if (!ServerHost) return;

	ENetPacket* ENetPacket = FPacketBuilder::BuildPacket(PacketData, bReliable);
	InternalSendToAll(ENetPacket);
}

void UNet6ServerSubsystem::Tick(float DeltaTime)
{
	if (!ServerHost) return;

	ENetEvent Event;

	// Boucle de traitement des �v�nements (Non-bloquante : timeout = 0)
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
			// Note : Event.packet sera d�truit automatiquement par notre helper
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

void UNet6ServerSubsystem::Deinitialize()
{
	// On coupe le serveur proprement quand on ferme le jeu ou l'éditeur
	StopServer();
	
	UE_LOG(LogExoServer, Log, TEXT("Subsystem désactivé : Ports libérés."));
	
	Super::Deinitialize();
}

void UNet6ServerSubsystem::HandleClientConnect(ENetPeer* Peer)
{
	uint32 NewID = NextClientID++;

	Peer->data = (void*)(uintptr_t)NewID;

	FConnectedClient NewClient;
	NewClient.Peer = Peer;
	NewClient.InternalID = NewID;
	NewClient.Status = EExoPlayerStatus::Connecting;

	// Add to map
	ConnectedClients.Add(NewID, NewClient);
	UE_LOG(LogExoServer, Log, TEXT("Nouveau Client Connect� ! ID ENet : %u"), Peer->connectID);
	BP_OnClientConnectEvent(NewClient.InternalID, NewClient.Status);
}

void UNet6ServerSubsystem::HandleClientDisconnect(ENetPeer* Peer)
{
	uint32 PlayerID = (uint32)(uintptr_t)Peer->data;

	UE_LOG(LogExoServer, Log, TEXT("Client D�connect� ID: %u"), PlayerID);

	// On retire Player de la map
	if (ConnectedClients.Contains(PlayerID))
	{
		ConnectedClients.Remove(PlayerID);
	}
	BP_OnClientDisconnectEvent(PlayerID);
	Peer->data = nullptr;
}

void UNet6ServerSubsystem::HandleReceivePacket(ENetPeer* Peer, const ENetPacket* Packet)
{
	// Sécurité de base
	if (!Peer || !Packet) return;
	// 1. On récupère l'ID du joueur qui a envoyé le message
	uint32 PlayerID = (uint32)(uintptr_t)Peer->data;
	BP_OnReceivePacketEvent(PlayerID);
	
	// 3. On passe cette fenêtre au routeur
	ExoHunterOpcodeRouter::RouteClientMessage(GetWorld(), PlayerID, Packet);

	// 4. Le routeur a terminé, on peut nettoyer la mémoire d'ENet.
	// (Le const_cast est nécessaire car on met "const ENetPacket*" dans ton .h, 
	// mais ENet a besoin d'un pointeur modifiable pour le détruire).
	enet_packet_destroy(const_cast<ENetPacket*>(Packet));
}

void UNet6ServerSubsystem::InternalSendToPlayer(uint32 PlayerID, ENetPacket* PacketToSend)
{
	if (!PacketToSend || !ServerHost) return;

	// 1. On cherche le joueur
	FConnectedClient* Player = ConnectedClients.Find(PlayerID);

	// 2. Si le joueur existe et a un Peer valide, on envoie
	if (Player && Player->Peer)
	{
		enet_peer_send(Player->Peer, 0, PacketToSend);
	}
	else
	{
		// 3. 🚨 LE JOUEUR N'EXISTE PLUS ! 
		// ENet ne gérera pas ce paquet, on doit le détruire nous-mêmes pour éviter la fuite
		UE_LOG(LogExoServer, Warning, TEXT("Échec d'envoi : Le joueur %u est introuvable."), PlayerID);
		enet_packet_destroy(PacketToSend);
	}
}

void UNet6ServerSubsystem::InternalSendToAll(ENetPacket* PacketToSend)
{
	if (!PacketToSend || !ServerHost) return;

	// Broadcast est ultra optimisé : il envoie le même pointeur à tout le monde
	// et ENet le détruira automatiquement quand le dernier client l'aura reçu.
	enet_host_broadcast(ServerHost, 0, PacketToSend);
}