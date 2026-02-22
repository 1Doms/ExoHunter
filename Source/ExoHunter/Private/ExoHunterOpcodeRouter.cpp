#include "ExoHunterOpcodeRouter.h"
#include "ExoHunterCommandHandler.h"
#include "enet6/enet.h"
#include "ExoHunterTypes.h"
#include "PacketBuilder.h"

void ExoHunterOpcodeRouter::RouteClientMessage(UWorld* World, uint32 PlayerID, const ENetPacket *Packet)
{
	EExoHunterOpcode Opcode;
	TArrayView<const uint8> Payload;
	
	// Sépare l'Opcode, et le reste de la struct dans Payload
	if (!ExtractOpcodeAndPayload(Packet, Opcode, Payload))
		return;
	
	// La lambda : [&] capture toutes les variables locales (World, PlayerID, Payload) par référence"
	auto Dispatch = [&](auto& PacketStruct) 
	{
		int32 Offset = 0;
    
		// 1. Elle lit la struct (grâce au paramètre auto, elle s'adapte à n'importe quel type de paquet !)
		UNetworkSerializationLib::ReadStruct(Payload, Offset, PacketStruct);
		// On remplie ma structure qu'on met par valeur avec Payload qui est Deserialize
		
		// 2. Elle appelle le bon Handler grâce à la surcharge (overload)
		ExoHunterCommandHandler::HandlePacket(World, PlayerID, PacketStruct);
	};
	
	// 3. Routage
	switch (Opcode)
	{
	case EExoHunterOpcode::C_Connect:
		{
			FClientConnectPacket Packet;

			UE_LOG(LogTemp, Log, TEXT("Serveur: Le joueur %u s'appelle %s"), PlayerID, *Packet.playerName);
			Dispatch(Packet);
			break;
		}
	case EExoHunterOpcode::C_Disconnect:
		{
			
		}
	case EExoHunterOpcode::C_Input:
		{
			// FExoCommandHandler::HandleClientInput(PlayerID, Payload);
			break;
		}
	default:
		{
			UE_LOG(LogTemp, Warning, TEXT("Serveur: Opcode inconnu ou non autorisé (%d) depuis le joueur %u"), Packet[0], PlayerID);
			break;
		}
	}
}

void ExoHunterOpcodeRouter::RouteServerMessage(UWorld* World, const ENetPacket* Packet)
{
	EExoHunterOpcode Opcode;
	TArrayView<const uint8> Payload;
	
	if (!ExtractOpcodeAndPayload(Packet, Opcode, Payload))
		return;

	auto Dispatch = [&](auto& PacketStruct) 
	{
		int32 Offset = 0;
		UNetworkSerializationLib::ReadStruct(Payload, Offset, PacketStruct);
        
		// On appelle le Handler sans PlayerID
		ExoHunterCommandHandler::HandlePacket(World, PacketStruct);
	};
	
	switch (Opcode)
	{
	case EExoHunterOpcode::S_PlayerJoin:
		{
			FServerPlayerJoin Packet;
			UE_LOG(LogTemp, Log, TEXT("Client: Reçu S_PlayerJoin (Taille des données: %d octets)"), Payload.Num());
			Dispatch(Packet);
			break;
		}
	case EExoHunterOpcode::S_SpawnActor:
		{
			UE_LOG(LogTemp, Log, TEXT("Client: Reçu S_SpawnActor"));
			break;
		}
	default:
		{
			UE_LOG(LogTemp, Warning, TEXT("Client: Opcode inconnu reçu du serveur (%d)"), Packet[0]);
			break;
		}
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
