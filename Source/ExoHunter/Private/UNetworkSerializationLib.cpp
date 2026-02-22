#include "UNetworkSerializationLib.h"

// ==============================================================================
//                                HELPERS PRIV�S
// ==============================================================================

// Helper g�n�rique pour ajouter une donn�e � la FIN du tableau
template <typename T>
void WriteAppend(TArray<uint8>& byteArray, const T& Value)
{
	int32 NewIndex = byteArray.AddUninitialized(sizeof(T));
	FMemory::Memcpy(byteArray.GetData() + NewIndex, &Value, sizeof(T)); // byteArray.GetData() + NewIndex == byteArray[NewIndex]
}

// Helper g�n�rique pour �crire une donn�e � un OFFSET pr�cis (�crase ou agrandit)
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

// Helper g�n�rique pour lire une donn�e � un OFFSET (et avancer l'offset)
template <typename T>
	T ReadAtOffset(TArrayView<const uint8> ByteArrayView, int32& Offset)
{
	if (!UNetworkSerializationLib::HasBytesLeft(ByteArrayView, Offset, sizeof(T)))
	{
		return T(); 
	}
	T Value;
	FMemory::Memcpy(&Value, ByteArrayView.GetData() + Offset, sizeof(T));
	Offset += sizeof(T);
	return Value;
}


// ==============================================================================
//                                   �CRITURE (WRITE)
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
	else if (byteArray.Num() == Offset) // Cas o� on �crit juste � la fin
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

	// 1. �crire la taille
	WriteInt32(byteArray, Len);

	// 2. �crire le contenu
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

	// 1. �crire la taille � l'offset
	WriteInt32At(byteArray, Offset, Len);
	Offset += 4; // On avance de 4 octets (taille du int32)

	// 2. �crire le contenu
	if (Len > 0)
	{
		// On s'assure qu'il y a assez de place pour la cha�ne
		if (byteArray.Num() < Offset + Len)
		{
			byteArray.SetNum(Offset + Len);
		}
		FMemory::Memcpy(byteArray.GetData() + Offset, Convert.Get(), Len);
	}
}

// --- VECTOR & ROTATOR ---

void UNetworkSerializationLib::WriteVector(UPARAM(ref)TArray<uint8>& byteArray, const FVector& Value)
{
	// Conversion Double (UE5) -> Float (R�seau)
	WriteFloat(byteArray, (float)Value.X);
	WriteFloat(byteArray, (float)Value.Y);
	WriteFloat(byteArray, (float)Value.Z);
}

void UNetworkSerializationLib::WriteRotator(UPARAM(ref)TArray<uint8>& byteArray, const FRotator& Value)
{
	// Conversion Double (UE5) -> Float (R�seau)
	WriteFloat(byteArray, (float)Value.Pitch);
	WriteFloat(byteArray, (float)Value.Yaw);
	WriteFloat(byteArray, (float)Value.Roll);
}


// ==============================================================================
//                                   LECTURE (READ)
// ==============================================================================

int32 UNetworkSerializationLib::ReadInt8(TArrayView<const uint8> ByteArrayView, int32& Offset)
{
	if (!HasBytesLeft(ByteArrayView, Offset, 1)) return 0;

	int8 Val = (int8)ByteArrayView[Offset];
	Offset++;
	return (int32)Val;
}

uint8 UNetworkSerializationLib::ReadUint8(TArrayView<const uint8> ByteArrayView, int32& Offset)
{
	if (!HasBytesLeft(ByteArrayView, Offset, 1)) return 0;

	uint8 Val = ByteArrayView[Offset];
	Offset++;
	return Val;
}

int32 UNetworkSerializationLib::ReadInt16(TArrayView<const uint8> ByteArrayView, int32& Offset)
{
	return (int32)ReadAtOffset<int16>(ByteArrayView, Offset);
}

int32 UNetworkSerializationLib::ReadUint16(TArrayView<const uint8> ByteArrayView, int32& Offset)
{
	return (int32)ReadAtOffset<uint16>(ByteArrayView, Offset);
}

int32 UNetworkSerializationLib::ReadInt32(TArrayView<const uint8> ByteArrayView, int32& Offset)
{
	return ReadAtOffset<int32>(ByteArrayView, Offset);
}

int32 UNetworkSerializationLib::ReadUint32(TArrayView<const uint8> ByteArrayView, int32& Offset)
{
	return (int32)ReadAtOffset<uint32>(ByteArrayView, Offset);
}

float UNetworkSerializationLib::ReadFloat(TArrayView<const uint8> ByteArrayView, int32& Offset)
{
	return ReadAtOffset<float>(ByteArrayView, Offset);
}

FString UNetworkSerializationLib::ReadString(TArrayView<const uint8> ByteArrayView, int32& Offset)
{
	// 1. Lire la taille
	int32 Len = ReadInt32(ByteArrayView, Offset);

	// S�curit�
	if (Len <= 0 || !HasBytesLeft(ByteArrayView, Offset, Len))
	{
		return FString();
	}

	// 2. Lire le contenu et convertir UTF8 -> TCHAR
	FString Result(Len, (const UTF8CHAR*)(ByteArrayView.GetData() + Offset));
	Offset += Len;

	return Result;
}

FVector UNetworkSerializationLib::ReadVector(TArrayView<const uint8> ByteArrayView, int32& Offset)
{
	// Reconstitution du vecteur (Float -> Double implicite)
	float X = ReadFloat(ByteArrayView, Offset);
	float Y = ReadFloat(ByteArrayView, Offset);
	float Z = ReadFloat(ByteArrayView, Offset);
	return FVector(X, Y, Z);
}

FRotator UNetworkSerializationLib::ReadRotator(TArrayView<const uint8> ByteArrayView, int32& Offset)
{
	float P = ReadFloat(ByteArrayView, Offset);
	float Y = ReadFloat(ByteArrayView, Offset);
	float R = ReadFloat(ByteArrayView, Offset);
	return FRotator(P, Y, R);
}

bool UNetworkSerializationLib::HasBytesLeft(TArrayView<const uint8> Bytes, int32 Offset, int32 SizeRequired)
{
	return Bytes.IsValidIndex(Offset) && (Bytes.Num() - Offset) >= SizeRequired;
}

// Pour tous �l�ments d'une structure -> Serialize automatique sa propri�t� (VarType)
void UNetworkSerializationLib::WriteStructViaReflection(TArray<uint8>& byteArray, const UScriptStruct* StructDefinition, const void* StructData)
{
	// On it�re sur chaque propri�t� de la struct (Int, Float, String, etc.)
	for (TFieldIterator<FProperty> It(StructDefinition); It; ++It)
	{
		FProperty* Property = *It;

		// On r�cup�re le pointeur vers la valeur r�elle en m�moire
		// ContainerPtrToValuePtr fait le calcul d'adresse (Offset) pour nous
		const void* ValuePtr = Property->ContainerPtrToValuePtr<void>(StructData);

		// On d�l�gue l'�criture
		WriteProperty(byteArray, Property, ValuePtr);
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
		WriteUint32(Bytes, (int32)Val); // Attention au cast si > 2 milliards (faut g�rer le signe)
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
		WriteUint8(Bytes, Val ? 1 : 0); // Bool s�rialis� comme 1 byte
	}

	// --- 2. STRING ---

	else if (FStrProperty* StrProp = CastField<FStrProperty>(Property))
	{
		FString Val = StrProp->GetPropertyValue(ValuePtr);
		WriteString(Bytes, Val);
	}

	// --- 3. STRUCTS IMBRIQU�ES (R�cursion !) ---

	else if (FStructProperty* StructProp = CastField<FStructProperty>(Property))
	{
		// Optimisation : Si c'est un Vector/Rotator, on utilise tes fonctions optimis�es
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
			// Sinon r�cursion standard
			WriteStructViaReflection(Bytes, StructProp->Struct, ValuePtr);
		}
	}

	// --- 4. TABLEAUX (TArray) ---

	else if (FArrayProperty* ArrayProp = CastField<FArrayProperty>(Property))
	{
		// Helper pour manipuler les TArray g�n�riques
		FScriptArrayHelper Helper(ArrayProp, ValuePtr);

		// On �crit la taille du tableau (int32 ou uint16 selon tes besoins)
		WriteInt32(Bytes, Helper.Num());

		// On it�re sur chaque �l�ment du tableau
		for (int32 i = 0; i < Helper.Num(); ++i)
		{
			const void* ElementPtr = Helper.GetRawPtr(i);
			// On r�utilise la m�me logique pour chaque �l�ment !
			WriteProperty(Bytes, ArrayProp->Inner, ElementPtr);
		}
	}
}

void UNetworkSerializationLib::ReadStructViaReflection(TArrayView<const uint8> ByteArrayView, int32& Offset, const UScriptStruct* StructDefinition, void* StructData)
{
	for (TFieldIterator<FProperty> It(StructDefinition); It; ++It)
	{
		FProperty* Property = *It;
		void* ValuePtr = Property->ContainerPtrToValuePtr<void>(StructData);

		ReadProperty(ByteArrayView, Offset, Property, ValuePtr);
	}
}

void UNetworkSerializationLib::ReadProperty(TArrayView<const uint8> ByteArrayView, int32 & Offset, FProperty * Property, void* ValuePtr)
{
	// --- 1. PRIMITIVES (Int, Float, Bool, Byte) ---

	// Uint8 (Byte)
	if (FByteProperty* ByteProp = CastField<FByteProperty>(Property))
	{
		uint8 Val = ReadUint8(ByteArrayView, Offset);
		ByteProp->SetPropertyValue(ValuePtr, Val);
	}
	// Int8
	else if (FInt8Property* Int8Prop = CastField<FInt8Property>(Property))
	{
		// On lit un int32 (car ReadInt8 renvoie int32 pour les BP) et on cast en int8
		int32 Val = ReadInt8(ByteArrayView, Offset);
		Int8Prop->SetPropertyValue(ValuePtr, (int8)Val);
	}
	// Int16
	else if (FInt16Property* Int16Prop = CastField<FInt16Property>(Property))
	{
		int32 Val = ReadInt16(ByteArrayView, Offset);
		Int16Prop->SetPropertyValue(ValuePtr, (int16)Val);
	}
	// UInt16
	else if (FUInt16Property* UInt16Prop = CastField<FUInt16Property>(Property))
	{
		int32 Val = ReadUint16(ByteArrayView, Offset);
		UInt16Prop->SetPropertyValue(ValuePtr, (uint16)Val);
	}
	// Int32
	else if (FIntProperty* IntProp = CastField<FIntProperty>(Property))
	{
		int32 Val = ReadInt32(ByteArrayView, Offset);
		IntProp->SetPropertyValue(ValuePtr, Val);
	}
	// UInt32
	else if (FUInt32Property* UInt32Prop = CastField<FUInt32Property>(Property))
	{
		int32 Val = ReadUint32(ByteArrayView, Offset);
		UInt32Prop->SetPropertyValue(ValuePtr, (uint32)Val);
	}

	// Float
	else if (FFloatProperty* FloatProp = CastField<FFloatProperty>(Property))
	{
		float Val = ReadFloat(ByteArrayView, Offset);
		FloatProp->SetPropertyValue(ValuePtr, Val);
	}

	// Bool
	else if (FBoolProperty* BoolProp = CastField<FBoolProperty>(Property))
	{
		// On lit l'octet, si != 0 c'est true
		uint8 Val = ReadUint8(ByteArrayView, Offset);
		BoolProp->SetPropertyValue(ValuePtr, Val != 0);
	}

	// --- 2. STRING ---

	else if (FStrProperty* StrProp = CastField<FStrProperty>(Property))
	{
		FString Val = ReadString(ByteArrayView, Offset);
		StrProp->SetPropertyValue(ValuePtr, Val);
	}

	// --- 3. STRUCTS IMBRIQU�ES (R�cursion !) ---

	else if (FStructProperty* StructProp = CastField<FStructProperty>(Property))
	{
		// Optimisation Vector
		if (StructProp->Struct->GetFName() == NAME_Vector)
		{
			FVector Val = ReadVector(ByteArrayView, Offset);
			// On cast le pointeur void* directement en FVector* pour l'assigner
			*(FVector*)ValuePtr = Val;
		}
		// Optimisation Rotator
		else if (StructProp->Struct->GetFName() == NAME_Rotator)
		{
			FRotator Val = ReadRotator(ByteArrayView, Offset);
			*(FRotator*)ValuePtr = Val;
		}
		else
		{
			// Sinon r�cursion standard vers la fonction qui it�re sur les champs
			ReadStructViaReflection(ByteArrayView, Offset, StructProp->Struct, ValuePtr);
		}
	}

	// --- ARRAY ---
	else if (FArrayProperty* ArrayProp = CastField<FArrayProperty>(Property))
	{
		FScriptArrayHelper Helper(ArrayProp, ValuePtr);

		// 1. Lire la taille
		int32 ArraySize = ReadInt32(ByteArrayView, Offset);

		// 2. Redimensionner le tableau pour accueillir les donn�es
		Helper.Resize(ArraySize);

		// 3. Remplir chaque �l�ment
		for (int32 i = 0; i < ArraySize; ++i)
		{
			// Appel r�cursif sur la propri�t� interne (Inner) pour l'�l�ment i
			ReadProperty(ByteArrayView, Offset, ArrayProp->Inner, Helper.GetRawPtr(i));
		}
	}
}