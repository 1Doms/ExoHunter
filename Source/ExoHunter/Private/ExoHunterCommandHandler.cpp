#include "ExoHunterCommandHandler.h"
#include "AExoHunterGameMode.h"
#include "AExoHunterGameState.h"
#include "PacketBuilder.h"
#include "Engine/World.h"

// =============================================================================
// HELPERS
// =============================================================================

AExoHunterGameMode* ExoHunterCommandHandler::GetExoGameMode(UWorld* World)
{
	if (!World) return nullptr;
	return Cast<AExoHunterGameMode>(World->GetAuthGameMode());
}

AExoHunterGameState* ExoHunterCommandHandler::GetExoGameState(UWorld* World)
{
	if (!World) return nullptr;
	return Cast<AExoHunterGameState>(World->GetGameState());
}

// =============================================================================
// SERVEUR : Gère les messages envoyés par les clients
// =============================================================================

void ExoHunterCommandHandler::HandleStruct(AExoHunterGameMode* GM, uint32 PlayerID, const FClientConnectStruct& Struct)
{
	UE_LOG(LogTemp, Warning, TEXT("CommandHandler : Commande de Connect du joueur %s !"), *Struct.PlayerName);
	GM->BP_OnPlayerValidated((int32)PlayerID, Struct);
}

void ExoHunterCommandHandler::HandleStruct(AExoHunterGameMode* GM, uint32 PlayerID, const FClientDisconnectStruct& Struct)
{
	UE_LOG(LogTemp, Warning, TEXT("CommandHandler : Le joueur %d s'est déconnecté."), PlayerID);
	// GM->..
}

void ExoHunterCommandHandler::HandleStruct(AExoHunterGameMode* GM, uint32 PlayerID, const FClientInputStruct& Struct)
{
	//Ici ca va être ma logique de calcul etc on dirait que CommandHandler ne sert à rien, car j'ai pas eu le temps de l'utiliser ;'(
}

void ExoHunterCommandHandler::HandleStruct(AExoHunterGameMode* GM, uint32 PlayerID, const FClientTest& Struct)
{
	
}

// =============================================================================
// CLIENT : Gère les messages envoyés par le serveur
// =============================================================================

void ExoHunterCommandHandler::HandleStruct(AExoHunterGameState* GS, const FServerClientConnectingStruct& Struct)
{
	GS->BP_OnClientConnecting(Struct);
}

void ExoHunterCommandHandler::HandleStruct(AExoHunterGameState* GS, const FServerPlayerJoinStruct& Struct)
{
	UE_LOG(LogTemp, Log, TEXT("CommandHandler : Un nouveau joueur a rejoint la partie !"));
}

void ExoHunterCommandHandler::HandleStruct(AExoHunterGameState* GS, const FServerSpawnActorStruct& Struct)
{
	UE_LOG(LogTemp, Warning, TEXT("CommandHandler : Ordre de Spawn reçu du serveur"));
}