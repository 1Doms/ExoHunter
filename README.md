# ExoHunter



Gameplay :



Architecture :



Server -> Net6ServerSubsystem -> OpcodeRouter -> CommandHandler -> GameMode (Validate) -> StateReplication



Client -> Net6ClientSubsystem -> OpcodeRouter -> CommandHandler -> StateReplication





other :



Enet6Lib -> Network lib



ExoHunterType.h -> Variable Enum, Struct

ExoHunterProtocol -> Opcode, PacketManager

NetworkSerialization -> Serialization binaire

Net6BaseSubsystem -> GameInstance Parent (child -> Server, Client)



