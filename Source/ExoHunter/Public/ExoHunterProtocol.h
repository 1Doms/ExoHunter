#pragma once

#include "CoreMinimal.h"
#include <enet6/enet.h>
#include "UNetworkSerializationLib.h"
#include "ExoHunterProtocol.generated.h"

// 1. Les Opcodes visibles en Blueprint
UENUM(BlueprintType)
enum class EExoHunterOpcode : uint8
{
    //Client to Server
    C_Connect = 0       UMETA(DisplayName = "Client Connect"),
    C_Disconnect = 1    UMETA(DisplayName = "Client Disconnect"),
    C_Input     UMETA(DisplayName = "Client Input"),

    //Server to client
    S_Welcome           UMETA(DisplayName = "[SV] Welcome / Join Success"),
    S_SpawnActor        UMETA(DisplayName = "[SV] Spawn Network Actor"),
    S_DestroyActor      UMETA(DisplayName = "[SV] Destroy Network Actor"),
    S_GameEvent         UMETA(DisplayName = "[SV] Gameplay Event"),
    S_Snapshot          UMETA(DisplayName = "[SV] World Snapshot"),
    S_MatchState        UMETA(DisplayName = "[SV] Match State Update")
};

template<typename TPacketStruct>
static ENetPacket* BuildPacket(const TPacketStruct& Packet, uint32 Flags)
{
    TArray<uint8> byteArray;

    UNetworkSerializationLib::WriteUint8(byteArray, (uint8)TPacketStruct::Opcode);

    UNetworkSerializationLib::WriteStruct(byteArray, Packet);

    return enet_packet_create(byteArray.GetData(), byteArray.Num(), Flags);
}