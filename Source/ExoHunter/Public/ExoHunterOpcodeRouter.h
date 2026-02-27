#pragma once

#include "CoreMinimal.h"
#include "UNet6ClientSubsystem.h"
#include "ExoHunterCommandHandler.h"

class ExoHunterOpcodeRouter
{
public:
	// --- Client to Server ---
	// Reçoit le paquet d'un client et l'ID de ce client pour savoir qui a parlé
	static void RouteClientMessage(UWorld* World, uint32 PlayerID, const ENetPacket* Packet);

	// --- Server to Client ---
	// Reçoit le paquet envoyé par le serveur
	static void RouteServerMessage(UWorld* World, const ENetPacket* Packet);
	
private:
	static bool ExtractOpcodeAndPayload(const ENetPacket* Packet, EExoHunterOpcode& OutOpcode, TArrayView<const uint8>& OutPayload);
	
	// Pour les paquets Client -> Serveur (avec PlayerID)
	template<typename TStruct>
	static TFunction<void(UWorld*, uint32, TArrayView<const uint8>&)> BindClientHandler()
	{
		return [](UWorld* World, uint32 PlayerID, TArrayView<const uint8>& Payload) 
		{
			
			AExoHunterGameMode* GM = ExoHunterCommandHandler::GetExoGameMode(World);
			if (!GM) return; // Si on n'est pas sur le serveur, on coupe tout !
			int32 Offset = 0;
			TStruct PacketStruct;
			UNetworkSerializationLib::ReadStruct(Payload, Offset, PacketStruct);
			ExoHunterCommandHandler::HandleStruct(GM, PlayerID, PacketStruct);
		};
	}

	// Pour les paquets Serveur -> Client (sans PlayerID)
	template<typename TStruct>
	static TFunction<void(UWorld*, TArrayView<const uint8>&)> BindServerHandler()
	{
		return [](UWorld* World, TArrayView<const uint8>& Payload) 
		{
			AExoHunterGameState* GS = ExoHunterCommandHandler::GetExoGameState(World);
			if (!GS) return;
			
			int32 Offset = 0;
			TStruct PacketStruct;
			UNetworkSerializationLib::ReadStruct(Payload, Offset, PacketStruct);
			ExoHunterCommandHandler::HandleStruct(GS, PacketStruct);
		};
	}
};
