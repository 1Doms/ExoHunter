#include "PacketBuilder.h"
#include "enet6/enet.h"

ENetPacket* FPacketBuilder::CreateENetPacketInternal(const uint8* Data, size_t DataSize, bool bReliable)
{
	uint32 Flags = bReliable ? ENET_PACKET_FLAG_RELIABLE : 0;
	return enet_packet_create(Data, DataSize, Flags);
}
