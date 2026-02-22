#pragma once

#include "CoreMinimal.h"

struct FClientConnectPacket;
struct FServerSpawnActorPacket;

class AExoHunterGameMode;

class ExoHunterCommandHandler
{
public:
	// On a besoin du UWorld* pour pouvoir trouver le GameMode !
	
	// --- SERVEUR : Gère les messages envoyés par les clients ---
	static void HandlePacket(UWorld* World, uint32 PlayerID, const FClientConnectPacket& Packet);

	// --- CLIENT : Gère les messages envoyés par le serveur ---
	static void HandleServerSpawnActor(UWorld* World, const FServerSpawnActorPacket& Packet);
	static void HandlePacket(UWorld* World, const FServerSpawnActorPacket& Packet);
	
private:
	static AExoHunterGameMode* GetExoGameMode(UWorld* World);
};