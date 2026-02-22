#include "ExoHunterCommandHandler.h"
#include "AExoHunterGameMode.h" // Pour connaître ton GameMode
#include "PacketBuilder.h"
#include "Engine/World.h"

// --- HELPERS PRIVÉS (Pour éviter la répétition) ---

static AExoHunterGameMode* GetExoGameMode(UWorld* World) {
	if (!World) return nullptr;
	return Cast<AExoHunterGameMode>(World->GetAuthGameMode());
}

// --- SERVEUR : Gère les messages envoyés par les clients ---

void ExoHunterCommandHandler::HandlePacket(UWorld* World, uint32 PlayerID, const FClientConnectPacket& Packet)
{
	if (AExoHunterGameMode* GM = GetExoGameMode(World))
	{
		UE_LOG(LogTemp, Warning, TEXT("CommandHandler : Commande de Spawn envoyée au Blueprint pour %s !"), *Packet.playerName);
		GM->BP_OnPlayerValidated((int32)PlayerID, Packet);
	}
}

// --- CLIENT : Gère les messages envoyés par le serveur ---

void ExoHunterCommandHandler::HandlePacket(UWorld* World, const FServerSpawnActorPacket& Packet)
{

	
	UE_LOG(LogTemp, Warning, TEXT("CommandHandler : Ordre de Spawn reçu du serveur pour le NetID %d"), Packet.NetID);
}
