#pragma once

#include "CoreMinimal.h"
#include "DiffResults.h"
#include "ExoHunterTypes.h"
#include "UNetworkSerializationLib.h"
#include "InstancedStruct.h"
#include "PacketBuilder.generated.h"

USTRUCT(BlueprintType)
struct FExoPacketBase
{
    GENERATED_BODY()
    EExoHunterOpcode PacketOpcode = EExoHunterOpcode::None;
    
    FExoPacketBase() = default; // Requis par Unreal
    FExoPacketBase(EExoHunterOpcode InOpcode) : PacketOpcode(InOpcode) {} // Le raccourci
};

USTRUCT(BlueprintType)
struct FExoClientPacket : public FExoPacketBase { GENERATED_BODY() }; // Pour les paquets Client -> Serveur

USTRUCT(BlueprintType)
struct FExoServerPacket : public FExoPacketBase { GENERATED_BODY() }; // Pour les paquets Serveur -> Client

// --- CLIENT PACKETS ---

USTRUCT(BlueprintType)
struct FClientConnectStruct : public FExoClientPacket
{
    GENERATED_BODY()
    FClientConnectStruct() : FExoClientPacket(EExoHunterOpcode::C_Connect) {}
    
    UPROPERTY(BlueprintReadWrite, Category = "ExoNetwork|Struct")
    FString PlayerName;
};

USTRUCT(BlueprintType)
struct FClientDisconnectStruct : public FExoClientPacket
{
    GENERATED_BODY()
    FClientDisconnectStruct() : FExoClientPacket(EExoHunterOpcode::C_Disconnect) {}
};

USTRUCT(BlueprintType)
struct FClientInputStruct : public FExoClientPacket
{
    GENERATED_BODY()
    FClientInputStruct() : FExoClientPacket(EExoHunterOpcode::C_Input) {}
    
    uint32 SequenceNumber = 0;
    
    UPROPERTY(BlueprintReadWrite, Category = "ExoNetwork|Input")
    float InputDeltaTime = 0.0f;
    
    UPROPERTY(BlueprintReadWrite, Category = "ExoNetwork|Input")
    int32 CompressedInputs = 0;
    
    UPROPERTY(BlueprintReadWrite, Category = "ExoNetwork|Input")
    float CameraPitch = 0.0f; // Regarder en haut/bas

    UPROPERTY(BlueprintReadWrite, Category = "ExoNetwork|Input")
    float CameraYaw = 0.0f;   // Tourner à gauche/droite
};

// --- SERVER PACKETS ---

USTRUCT(BlueprintType)
struct FServerClientConnectingStruct : public FExoServerPacket
{
    GENERATED_BODY()
    FServerClientConnectingStruct() : FExoServerPacket(EExoHunterOpcode::S_ClientConnecting) {}
    
    UPROPERTY(BlueprintReadWrite, Category = "ExoNetwork|Struct")
    int32 NetID;
    
    UPROPERTY(BlueprintReadWrite, Category = "ExoNetwork|Struct")
    uint8 PlayerStatus;
};

USTRUCT(BlueprintType)
struct FServerPlayerJoinStruct : public FExoServerPacket
{
    GENERATED_BODY()
    FServerPlayerJoinStruct() : FExoServerPacket(EExoHunterOpcode::S_PlayerJoin) {}
    
    UPROPERTY(BlueprintReadWrite, Category = "ExoNetwork|Struct")
    FString playerName;
};

USTRUCT(BlueprintType)
struct FServerSpawnActorStruct : public FExoServerPacket
{
    GENERATED_BODY()
    FServerSpawnActorStruct() : FExoServerPacket(EExoHunterOpcode::S_SpawnActor) {}

    // L'ID réseau unique de l'entité qui vient d'apparaître
    UPROPERTY(BlueprintReadWrite, Category = "ExoNetwork|Struct")
    int32 NetID;
    
    // BP_ExoHunterCharacter
    UPROPERTY(BlueprintReadWrite, Category = "ExoNetwork|Struct")
    uint8 ClassID;
    
    // Sa position de départ
    UPROPERTY(BlueprintReadWrite, Category = "ExoNetwork|Struct")
    FVector Location;

    // Sa rotation de départ
    UPROPERTY(BlueprintReadWrite, Category = "ExoNetwork|Struct")
    FRotator Rotation;
};

struct _ENetPacket;
typedef _ENetPacket ENetPacket;

class FPacketBuilder
{
public:
    static ENetPacket* BuildPacket(const FInstancedStruct& PacketData, bool bReliable);

private:
    // Le header sait que ENetPacket existe, donc il accepte le pointeur en retour
    static ENetPacket* CreateENetPacketInternal(const uint8* Data, size_t DataSize, bool bReliable);
};