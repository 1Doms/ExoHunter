#include "PacketBuilder.h"
#include "enet6/enet.h"

ENetPacket* FPacketBuilder::BuildPacket(const FInstancedStruct& PacketData, bool bReliable)
{
	const UScriptStruct* StructType = PacketData.GetScriptStruct(); // Le plan de montage (UScriptStruct*).
	const uint8* Memory = PacketData.GetMemory(); // Les données brutes (void*).

	if (!StructType || !Memory) return nullptr;

	// 1. EXTRACTION DE L'OPCODE
	// Cast magique vers la base commune pour lire la variable interne
	const FExoPacketBase* BasePacket = reinterpret_cast<const FExoPacketBase*>(Memory);
	EExoHunterOpcode Opcode = BasePacket->PacketOpcode;

	if (Opcode == EExoHunterOpcode::None)
	{
		UE_LOG(LogTemp, Error, TEXT("BuildPacket: Tentative de sérialisation d'un paquet sans Opcode !"));
		return nullptr;
	}

	// 2. PRÉPARATION DU BUFFER
	TArray<uint8> ByteArray;
	
	// On écrit l'Opcode au tout début
	UNetworkSerializationLib::WriteUint8(ByteArray, static_cast<uint8>(Opcode));

	// 3. SÉRIALISATION BLUEPRINT (Ignorera l'Opcode car pas de UPROPERTY)
	UNetworkSerializationLib::WriteStructViaReflection(ByteArray, StructType, Memory);

	return CreateENetPacketInternal(ByteArray.GetData(), ByteArray.Num(), bReliable);
}

ENetPacket* FPacketBuilder::CreateENetPacketInternal(const uint8* Data, size_t DataSize, bool bReliable)
{
	uint32 Flags = bReliable ? ENET_PACKET_FLAG_RELIABLE : 0;
	return enet_packet_create(Data, DataSize, Flags);
}
