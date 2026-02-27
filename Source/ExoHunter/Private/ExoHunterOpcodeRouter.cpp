#include "ExoHunterOpcodeRouter.h"
#include "ExoHunterCommandHandler.h"
#include "enet6/enet.h"
#include "ExoHunterTypes.h"
#include "PacketBuilder.h"

// --- Client to Server ---
// Reçoit le paquet d'un client et l'ID de ce client pour savoir qui a parlé
void ExoHunterOpcodeRouter::RouteClientMessage(UWorld* World, uint32 PlayerID, const ENetPacket *Packet)
{
	EExoHunterOpcode Opcode;
	TArrayView<const uint8> Payload;
	if (!ExtractOpcodeAndPayload(Packet, Opcode, Payload)) return;

	// 1. Le Tableau de Routage (Initialisé une seule fois)
	// l'Opcode est la clé 
	static TMap<EExoHunterOpcode, TFunction<void(UWorld*, uint32, TArrayView<const uint8>&)>> DispatchTable;
	if (DispatchTable.IsEmpty())
	{
		DispatchTable.Add(EExoHunterOpcode::C_Connect, BindClientHandler<FClientConnectStruct>());
		DispatchTable.Add(EExoHunterOpcode::C_Disconnect, BindClientHandler<FClientDisconnectStruct>());
		DispatchTable.Add(EExoHunterOpcode::C_Input, BindClientHandler<FClientInputStruct>());
	}
	
	// Si DispatchTable contient l'Opcode éxécute la lambda
	if (DispatchTable.Contains(Opcode)) {
		DispatchTable[Opcode](World, PlayerID, Payload);
	} else {
		UE_LOG(LogTemp, Warning, TEXT("Serveur: Opcode inconnu (%d) depuis le joueur %u"), static_cast<uint8>(Opcode), PlayerID);
	}
}

// --- Server to Client ---
// Reçoit le paquet envoyé par le serveur
void ExoHunterOpcodeRouter::RouteServerMessage(UWorld* World, const ENetPacket* Packet)
{
	EExoHunterOpcode Opcode;
	TArrayView<const uint8> Payload;
	if (!ExtractOpcodeAndPayload(Packet, Opcode, Payload)) return;

	// 1. Le Tableau de Routage Client
	static TMap<EExoHunterOpcode, TFunction<void(UWorld*, TArrayView<const uint8>&)>> DispatchTable;
	if (DispatchTable.IsEmpty())
	{
		// N'oublie pas d'ajouter les autres quand tu les auras codés !
		DispatchTable.Add(EExoHunterOpcode::S_ClientConnecting, BindServerHandler<FServerClientConnectingStruct>());
		DispatchTable.Add(EExoHunterOpcode::S_PlayerJoin, BindServerHandler<FServerPlayerJoinStruct>());
		DispatchTable.Add(EExoHunterOpcode::S_SpawnActor, BindServerHandler<FServerSpawnActorStruct>());
	}

	// 2. L'exécution
	if (DispatchTable.Contains(Opcode)) {
		DispatchTable[Opcode](World, Payload);
	} else {
		UE_LOG(LogTemp, Warning, TEXT("Client: Opcode inconnu reçu du serveur (%d)"), static_cast<uint8>(Opcode));
	}
}
bool ExoHunterOpcodeRouter::ExtractOpcodeAndPayload(const ENetPacket* Packet, EExoHunterOpcode& OutOpcode,TArrayView<const uint8>& OutPayload)
{
	// Sécurité de base
	if (!Packet || Packet->dataLength < 1)
	{
		return false;
	}

	// ZERO-COPY : création de la vue globale
	TArrayView<const uint8> PacketData(Packet->data, Packet->dataLength);

	// Extraction
	OutOpcode = static_cast<EExoHunterOpcode>(PacketData[0]);
	OutPayload = PacketData.RightChop(1);

	return true;
}
