#include "UNetworkSerializationLib.h"

// ==============================================================================
//                                HELPERS PRIVÉS
// ==============================================================================

// Helper générique pour ajouter une donnée à la FIN du tableau
template <typename T>
void WriteAppend(TArray<uint8>& byteArray, const T& Value)
{
	int32 NewIndex = byteArray.AddUninitialized(sizeof(T));
	FMemory::Memcpy(byteArray.GetData() + NewIndex, &Value, sizeof(T)); // byteArray.GetData() + NewIndex == byteArray[NewIndex]
}

// Helper générique pour écrire une donnée à un OFFSET précis (écrase ou agrandit)
template <typename T>
void WriteAtOffset(TArray<uint8>& byteArray, int32 Offset, const T& Value)
{
	// On s'assure que le tableau est assez grand
	if (byteArray.Num() < Offset + sizeof(T))
	{
		byteArray.SetNum(Offset + sizeof(T));
	}

	FMemory::Memcpy(byteArray.GetData() + Offset, &Value, sizeof(T));
}

// Helper générique pour lire une donnée à un OFFSET (et avancer l'offset)
template <typename T>
T ReadAtOffset(const TArray<uint8>& byteArray, int32& Offset)
{
	// Sécurité : On vérifie s'il reste assez d'octets
	if (!UNetworkSerializationLib::HasBytesLeft(byteArray, Offset, sizeof(T)))
	{
		return T(); // Retourne une valeur par défaut (0) en cas d'erreur
	}

	T Value;
	FMemory::Memcpy(&Value, byteArray.GetData() + Offset, sizeof(T));
	Offset += sizeof(T);
	return Value;
}


// ==============================================================================
//                                   ÉCRITURE (WRITE)
// ==============================================================================

// --- 8 BITS ---

void UNetworkSerializationLib::WriteInt8(UPARAM(ref)TArray<uint8>& byteArray, int32 Value)
{
	byteArray.Add((int8)Value);
}

void UNetworkSerializationLib::WriteInt8At(UPARAM(ref)TArray<uint8>& byteArray, int32 Offset, int32 Value)
{
	if (byteArray.IsValidIndex(Offset))
	{
		byteArray[Offset] = (int8)Value;
	}
	else if (byteArray.Num() == Offset) // Cas où on écrit juste à la fin
	{
		byteArray.Add((int8)Value);
	}
}

void UNetworkSerializationLib::WriteUint8(UPARAM(ref)TArray<uint8>& byteArray, uint8 Value)
{
	byteArray.Add(Value);
}

void UNetworkSerializationLib::WriteUint8At(UPARAM(ref)TArray<uint8>& byteArray, int32 Offset, uint8 Value)
{
	if (byteArray.IsValidIndex(Offset))
	{
		byteArray[Offset] = Value;
	}
	else if (byteArray.Num() == Offset)
	{
		byteArray.Add(Value);
	}
}

// --- 16 BITS ---

void UNetworkSerializationLib::WriteInt16(UPARAM(ref)TArray<uint8>& byteArray, int32 Value)
{
	int16 Val = (int16)Value;
	WriteAppend(byteArray, Val);
}

void UNetworkSerializationLib::WriteInt16At(UPARAM(ref)TArray<uint8>& byteArray, int32 Offset, int32 Value)
{
	int16 Val = (int16)Value;
	WriteAtOffset(byteArray, Offset, Val);
}

void UNetworkSerializationLib::WriteUint16(UPARAM(ref)TArray<uint8>& byteArray, int32 Value)
{
	uint16 Val = (uint16)Value;
	WriteAppend(byteArray, Val);
}

void UNetworkSerializationLib::WriteUint16At(UPARAM(ref)TArray<uint8>& byteArray, int32 Offset, int32 Value)
{
	uint16 Val = (uint16)Value;
	WriteAtOffset(byteArray, Offset, Val);
}

// --- 32 BITS ---

void UNetworkSerializationLib::WriteInt32(UPARAM(ref)TArray<uint8>& byteArray, int32 Value)
{
	WriteAppend(byteArray, Value);
}

void UNetworkSerializationLib::WriteInt32At(UPARAM(ref)TArray<uint8>& byteArray, int32 Offset, int32 Value)
{
	WriteAtOffset(byteArray, Offset, Value);
}

void UNetworkSerializationLib::WriteUint32(UPARAM(ref)TArray<uint8>& byteArray, int32 Value)
{
	uint32 Val = (uint32)Value;
	WriteAppend(byteArray, Val);
}

void UNetworkSerializationLib::WriteUint32At(UPARAM(ref)TArray<uint8>& byteArray, int32 Offset, int32 Value)
{
	uint32 Val = (uint32)Value;
	WriteAtOffset(byteArray, Offset, Val);
}

void UNetworkSerializationLib::WriteFloat(UPARAM(ref)TArray<uint8>& byteArray, float Value)
{
	WriteAppend(byteArray, Value);
}

void UNetworkSerializationLib::WriteFloatAt(UPARAM(ref)TArray<uint8>& byteArray, int32 Offset, float Value)
{
	WriteAtOffset(byteArray, Offset, Value);
}

// --- STRING ---

void UNetworkSerializationLib::WriteString(UPARAM(ref)TArray<uint8>& byteArray, const FString& Value)
{
	// Conversion Unreal TCHAR -> UTF8
	FTCHARToUTF8 Convert(*Value);
	int32 Len = Convert.Length();

	// 1. Écrire la taille
	WriteInt32(byteArray, Len);

	// 2. Écrire le contenu
	if (Len > 0)
	{
		int32 NewIndex = byteArray.AddUninitialized(Len);
		FMemory::Memcpy(byteArray.GetData() + NewIndex, Convert.Get(), Len);
	}
}

void UNetworkSerializationLib::WriteStringAt(UPARAM(ref)TArray<uint8>& byteArray, int32 Offset, const FString& Value)
{
	FTCHARToUTF8 Convert(*Value);
	int32 Len = Convert.Length();

	// 1. Écrire la taille à l'offset
	WriteInt32At(byteArray, Offset, Len);
	Offset += 4; // On avance de 4 octets (taille du int32)

	// 2. Écrire le contenu
	if (Len > 0)
	{
		// On s'assure qu'il y a assez de place pour la chaîne
		if (byteArray.Num() < Offset + Len)
		{
			byteArray.SetNum(Offset + Len);
		}
		FMemory::Memcpy(byteArray.GetData() + Offset, Convert.Get(), Len);
	}
}

// --- VECTOR & ROTATOR ---

void UNetworkSerializationLib::WriteVector(UPARAM(ref)TArray<uint8>& Bytes, const FVector& Value)
{
	// Conversion Double (UE5) -> Float (Réseau)
	WriteFloat(Bytes, (float)Value.X);
	WriteFloat(Bytes, (float)Value.Y);
	WriteFloat(Bytes, (float)Value.Z);
}

void UNetworkSerializationLib::WriteRotator(UPARAM(ref)TArray<uint8>& Bytes, const FRotator& Value)
{
	// Conversion Double (UE5) -> Float (Réseau)
	WriteFloat(Bytes, (float)Value.Pitch);
	WriteFloat(Bytes, (float)Value.Yaw);
	WriteFloat(Bytes, (float)Value.Roll);
}


// ==============================================================================
//                                   LECTURE (READ)
// ==============================================================================

int32 UNetworkSerializationLib::ReadInt8(const TArray<uint8>& byteArray, int32& Offset)
{
	if (!HasBytesLeft(byteArray, Offset, 1)) return 0;

	int8 Val = (int8)byteArray[Offset];
	Offset++;
	return (int32)Val;
}

uint8 UNetworkSerializationLib::ReadUint8(const TArray<uint8>& byteArray, int32& Offset)
{
	if (!HasBytesLeft(byteArray, Offset, 1)) return 0;

	uint8 Val = byteArray[Offset];
	Offset++;
	return Val;
}

int32 UNetworkSerializationLib::ReadInt16(const TArray<uint8>& byteArray, int32& Offset)
{
	return (int32)ReadAtOffset<int16>(byteArray, Offset);
}

int32 UNetworkSerializationLib::ReadUint16(const TArray<uint8>& byteArray, int32& Offset)
{
	return (int32)ReadAtOffset<uint16>(byteArray, Offset);
}

int32 UNetworkSerializationLib::ReadInt32(const TArray<uint8>& byteArray, int32& Offset)
{
	return ReadAtOffset<int32>(byteArray, Offset);
}

int32 UNetworkSerializationLib::ReadUint32(const TArray<uint8>& byteArray, int32& Offset)
{
	return (int32)ReadAtOffset<uint32>(byteArray, Offset);
}

float UNetworkSerializationLib::ReadFloat(const TArray<uint8>& byteArray, int32& Offset)
{
	return ReadAtOffset<float>(byteArray, Offset);
}

FString UNetworkSerializationLib::ReadString(const TArray<uint8>& byteArray, int32& Offset)
{
	// 1. Lire la taille
	int32 Len = ReadInt32(byteArray, Offset);

	// Sécurité
	if (Len <= 0 || !HasBytesLeft(byteArray, Offset, Len))
	{
		return FString();
	}

	// 2. Lire le contenu et convertir UTF8 -> TCHAR
	FString Result(Len, (const UTF8CHAR*)(byteArray.GetData() + Offset));
	Offset += Len;

	return Result;
}

FVector UNetworkSerializationLib::ReadVector(const TArray<uint8>& Bytes, int32& Offset)
{
	// Reconstitution du vecteur (Float -> Double implicite)
	float X = ReadFloat(Bytes, Offset);
	float Y = ReadFloat(Bytes, Offset);
	float Z = ReadFloat(Bytes, Offset);
	return FVector(X, Y, Z);
}

FRotator UNetworkSerializationLib::ReadRotator(const TArray<uint8>& Bytes, int32& Offset)
{
	float P = ReadFloat(Bytes, Offset);
	float Y = ReadFloat(Bytes, Offset);
	float R = ReadFloat(Bytes, Offset);
	return FRotator(P, Y, R);
}

bool UNetworkSerializationLib::HasBytesLeft(const TArray<uint8>& byteArray, int32 Offset, int32 SizeRequired)
{
	return byteArray.IsValidIndex(Offset) && (byteArray.Num() - Offset) >= SizeRequired;
}

void UNetworkSerializationLib::WriteStructViaReflection(TArray<uint8>& Bytes, const UScriptStruct* StructDefinition, const void* StructData)
{
	// On itère sur chaque propriété de la struct (Int, Float, String, etc.)
	for (TFieldIterator<FProperty> It(StructDefinition); It; ++It)
	{
		FProperty* Property = *It;

		// On récupère le pointeur vers la valeur réelle en mémoire
		// ContainerPtrToValuePtr fait le calcul d'adresse (Offset) pour nous
		const void* ValuePtr = Property->ContainerPtrToValuePtr<void>(StructData);

		// On délègue l'écriture
		WriteProperty(Bytes, Property, ValuePtr);
	}
}

void UNetworkSerializationLib::WriteProperty(TArray<uint8>& Bytes, FProperty* Property, const void* ValuePtr)
{
	// --- 1. PRIMITIVES (Int, Float, Bool, Byte) ---

	// Uint8
	if (FByteProperty* ByteProp = CastField<FByteProperty>(Property))
	{
		uint8 Val = ByteProp->GetPropertyValue(ValuePtr);
		WriteUint8(Bytes, Val);
	}
	// Int8
	else if (FInt8Property* Int8Prop = CastField<FInt8Property>(Property))
	{
		int8 Val = Int8Prop->GetPropertyValue(ValuePtr);
		WriteInt8(Bytes, (int32)Val);
	}
	// Int16
	else if (FInt16Property* Int16Prop = CastField<FInt16Property>(Property))
	{
		int16 Val = Int16Prop->GetPropertyValue(ValuePtr);
		WriteInt16(Bytes, (int32)Val);
	}
	// UInt16
	else if (FUInt16Property* UInt16Prop = CastField<FUInt16Property>(Property))
	{
		uint16 Val = UInt16Prop->GetPropertyValue(ValuePtr);
		WriteUint16(Bytes, (int32)Val);
	}
	// Int32
	else if (FIntProperty* IntProp = CastField<FIntProperty>(Property))
	{
		int32 Val = IntProp->GetPropertyValue(ValuePtr);
		WriteInt32(Bytes, Val);
	}
	// UInt32
	else if (FUInt32Property* UInt32Prop = CastField<FUInt32Property>(Property))
	{
		uint32 Val = UInt32Prop->GetPropertyValue(ValuePtr);
		WriteUint32(Bytes, (int32)Val); // Attention au cast si > 2 milliards (faut gérer le signe)
	}

	// Float
	else if (FFloatProperty* FloatProp = CastField<FFloatProperty>(Property))
	{
		float Val = FloatProp->GetPropertyValue(ValuePtr);
		WriteFloat(Bytes, Val);
	}

	// Bool
	else if (FBoolProperty* BoolProp = CastField<FBoolProperty>(Property))
	{
		bool Val = BoolProp->GetPropertyValue(ValuePtr);
		WriteUint8(Bytes, Val ? 1 : 0); // Bool sérialisé comme 1 byte
	}

	// --- 2. STRING ---

	else if (FStrProperty* StrProp = CastField<FStrProperty>(Property))
	{
		FString Val = StrProp->GetPropertyValue(ValuePtr);
		WriteString(Bytes, Val);
	}

	// --- 3. STRUCTS IMBRIQUÉES (Récursion !) ---

	else if (FStructProperty* StructProp = CastField<FStructProperty>(Property))
	{
		// Optimisation : Si c'est un Vector/Rotator, on utilise tes fonctions optimisées
		if (StructProp->Struct->GetFName() == NAME_Vector)
		{
			FVector* Vec = (FVector*)ValuePtr;
			WriteVector(Bytes, *Vec);
		}
		else if (StructProp->Struct->GetFName() == NAME_Rotator)
		{
			FRotator* Rot = (FRotator*)ValuePtr;
			WriteRotator(Bytes, *Rot);
		}
		else
		{
			// Sinon récursion standard
			WriteStructViaReflection(Bytes, StructProp->Struct, ValuePtr);
		}
	}

	// --- 4. TABLEAUX (TArray) ---

	else if (FArrayProperty* ArrayProp = CastField<FArrayProperty>(Property))
	{
		// Helper pour manipuler les TArray génériques
		FScriptArrayHelper Helper(ArrayProp, ValuePtr);

		// On écrit la taille du tableau (int32 ou uint16 selon tes besoins)
		WriteInt32(Bytes, Helper.Num());

		// On itère sur chaque élément du tableau
		for (int32 i = 0; i < Helper.Num(); ++i)
		{
			const void* ElementPtr = Helper.GetRawPtr(i);
			// On réutilise la même logique pour chaque élément !
			WriteProperty(Bytes, ArrayProp->Inner, ElementPtr);
		}
	}
}