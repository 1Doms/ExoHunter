#include "AExoHunterGameMode.h"
#include "UNet6ServerSubsystem.h"

void AExoHunterGameMode::BroadcastPlayerSpawned(const FServerSpawnActorPacket& SpawnPacket)
{
	if (UNet6ServerSubsystem* Net = GetServerSubsystem())
	{
		Net->SendToAllPlayers(SpawnPacket, true);
		UE_LOG(LogTemp, Warning, TEXT("Serveur : Ordre de spawn envoyé pour le NetID %d"), SpawnPacket.NetID);
	}
}
// --- 
UNet6ServerSubsystem* AExoHunterGameMode::GetServerSubsystem() const
{
	if (UGameInstance* GI = GetGameInstance())
	{
		return GI->GetSubsystem<UNet6ServerSubsystem>();
	}
	return nullptr;
}
