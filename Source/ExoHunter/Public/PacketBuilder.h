#pragma once

#include "CoreMinimal.h"
#include "ExoHunterTypes.h"
#include "UNetworkSerializationLib.h"
#include "PacketBuilder.generated.h"

// --- CLIENT PACKETS ---

USTRUCT(BlueprintType)
struct FClientConnectPacket
{
    GENERATED_BODY()

    static constexpr EExoHunterOpcode Opcode = EExoHunterOpcode::C_Connect;

    UPROPERTY(BlueprintReadWrite, Category = "ExoNetwork|Packets")
    FString playerName;
};

USTRUCT()
struct FClientDisconnectPacket
{
    GENERATED_BODY()

    static constexpr EExoHunterOpcode Opcode = EExoHunterOpcode::C_Disconnect;

    UPROPERTY()
    uint8 playerID;
};

// --- SERVER PACKETS ---

USTRUCT(BlueprintType)
struct FServerPlayerJoin
{
    GENERATED_BODY()
    
    static constexpr EExoHunterOpcode Opcode = EExoHunterOpcode::S_PlayerJoin;
    
    UPROPERTY(BlueprintReadOnly, Category = "ExoNetwork|Packets")
    FString playerName;
};

USTRUCT(BlueprintType)
struct FServerSpawnActorPacket
{
    GENERATED_BODY()

    static constexpr EExoHunterOpcode Opcode = EExoHunterOpcode::S_SpawnActor;

    // L'ID réseau unique de l'entité qui vient d'apparaître
    UPROPERTY(BlueprintReadOnly, Category = "ExoNetwork|Packets")
    int32 NetID;
    
    // BP_ExoHunterCharacter
    UPROPERTY(BlueprintReadOnly, Category = "ExoNetwork|Packets")
    uint8 ClassID;
    
    // Sa position de départ
    UPROPERTY(BlueprintReadOnly, Category = "ExoNetwork|Packets")
    FVector Location;

    // Sa rotation de départ
    UPROPERTY(BlueprintReadOnly, Category = "ExoNetwork|Packets")
    FRotator Rotation;
};

struct _ENetPacket;
typedef _ENetPacket ENetPacket;

class FPacketBuilder
{
public:
    template<typename TPacketStruct>
    static ENetPacket* BuildPacket(const TPacketStruct& Packet, bool bReliable)
    {
        TArray<uint8> ByteArray;
        ByteArray.Reserve(sizeof(TPacketStruct) + 1);
        UNetworkSerializationLib::WriteUint8(ByteArray, (uint8)TPacketStruct::Opcode);
        UNetworkSerializationLib::WriteStruct(ByteArray, Packet);

        // On appelle la fonction de création qui est définie dans le .cpp
        return CreateENetPacketInternal(ByteArray.GetData(), ByteArray.Num(), bReliable);
    }

private:
    // Le header sait que ENetPacket existe, donc il accepte le pointeur en retour
    static ENetPacket* CreateENetPacketInternal(const uint8* Data, size_t DataSize, bool bReliable);
};