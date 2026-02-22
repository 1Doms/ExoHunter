#pragma once

#include "CoreMinimal.h"
#include "UNet6ClientSubsystem.h"

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
};
