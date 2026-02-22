#pragma once

#include "CoreMinimal.h"

struct FServerTest;
struct FClientTest;
// --- Client to Server ---
struct FClientConnectStruct;
struct FClientDisconnectStruct;
struct FClientInputStruct;
// --- Server to Client ---
struct FServerClientConnectingStruct;
struct FServerPlayerJoinStruct;
struct FServerSpawnActorStruct;

class AExoHunterGameMode;
class AExoHunterGameState;

class ExoHunterCommandHandler
{
public:
	// On a besoin du UWorld* pour pouvoir trouver le GameMode !
	static AExoHunterGameMode* GetExoGameMode(UWorld* World);
	static AExoHunterGameState* GetExoGameState(UWorld* World);
	
	// --- SERVEUR : Gère les messages envoyés par les clients ---
	static void HandleStruct(AExoHunterGameMode* GM, uint32 PlayerID, const FClientConnectStruct& Struct);
	static void HandleStruct(AExoHunterGameMode* GM, uint32 PlayerID, const FClientDisconnectStruct& Struct);
	static void HandleStruct(AExoHunterGameMode* GM, uint32 PlayerID, const FClientInputStruct& Struct);
	static void HandleStruct(AExoHunterGameMode* GM, uint32 PlayerID, const FClientTest& Struct);

	// --- CLIENT : Gère les messages envoyés par le serveur ---
	static void HandleStruct(AExoHunterGameState* GS, const FServerClientConnectingStruct& Struct);
	static void HandleStruct(AExoHunterGameState* GS, const FServerPlayerJoinStruct& Struct);
	static void HandleStruct(AExoHunterGameState* GS, const FServerSpawnActorStruct& Struct);
	static void HandleStruct(AExoHunterGameState* GS, const FServerTest& Struct);
	
	// Sécurité pour le SERVER (3 Parameters)
	template<typename T>
	static void HandleStruct(UWorld*, uint32 PlayerID, const T&)
	{
		FString StructName = T::StaticStruct()->GetName();
		UE_LOG(LogTemp, Error, TEXT("SERVEUR : HandleStruct manquant pour le joueur %d avec le paquet [%s] !"), PlayerID, *StructName);
	}
	// Sécurité pour le CLIENT (2 Parameters)
	template<typename T>
	static void HandleStruct(UWorld*, const T&)
	{
		FString StructName = T::StaticStruct()->GetName();
		UE_LOG(LogTemp, Error, TEXT("CLIENT : HandleStruct manquant pour le paquet [%s] !"), *StructName);
	}
};