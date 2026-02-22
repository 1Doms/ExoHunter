#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UnetworkSerializationLib.generated.h"

/**
 * UNetworkSerializationLib
 * * Biblioth�que statique pour la s�rialisation binaire (Raw Bytes).
 * Permet de convertir les types Unreal (High Level) en paquets r�seau (Low Level).
 * * OPTIMISATIONS :
 * - Les FVector (Double precision dans UE5) sont convertis en Float (Simple precision) pour gagner 50% de place.
 * - Les FRotator sont convertis en Float (ou Short compress� dans une version future).
 */

UCLASS()
class EXOHUNTER_API UNetworkSerializationLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	// ==============================================================================
	//                                   �CRITURE (WRITE)
	// ==============================================================================

	// --- 8 BITS (Char / Byte) ---

	UFUNCTION(BlueprintCallable, Category = "ExoNetwork|Write")
	static void WriteInt8(UPARAM(ref) TArray<uint8>& byteArray, int32 Value);

	UFUNCTION(BlueprintCallable, Category = "ExoNetwork|Write")
	static void WriteInt8At(UPARAM(ref) TArray<uint8>& byteArray, int32 Offset, int32 Value);

	UFUNCTION(BlueprintCallable, Category = "ExoNetwork|Write")
	static void WriteUint8(UPARAM(ref) TArray<uint8>& byteArray, uint8 Value);

	UFUNCTION(BlueprintCallable, Category = "ExoNetwork|Write")
	static void WriteUint8At(UPARAM(ref) TArray<uint8>& byteArray, int32 Offset, uint8 Value);


	// --- 16 BITS (Short) ---

	UFUNCTION(BlueprintCallable, Category = "ExoNetwork|Write")
	static void WriteInt16(UPARAM(ref) TArray<uint8>& byteArray, int32 Value);

	UFUNCTION(BlueprintCallable, Category = "ExoNetwork|Write")
	static void WriteInt16At(UPARAM(ref) TArray<uint8>& byteArray, int32 Offset, int32 Value);

	UFUNCTION(BlueprintCallable, Category = "ExoNetwork|Write")
	static void WriteUint16(UPARAM(ref) TArray<uint8>& byteArray, int32 Value);

	UFUNCTION(BlueprintCallable, Category = "ExoNetwork|Write")
	static void WriteUint16At(UPARAM(ref) TArray<uint8>& byteArray, int32 Offset, int32 Value);


	// --- 32 BITS (Int / Float) ---

	UFUNCTION(BlueprintCallable, Category = "ExoNetwork|Write")
	static void WriteInt32(UPARAM(ref) TArray<uint8>& byteArray, int32 Value);

	UFUNCTION(BlueprintCallable, Category = "ExoNetwork|Write")
	static void WriteInt32At(UPARAM(ref) TArray<uint8>& byteArray, int32 Offset, int32 Value);

	UFUNCTION(BlueprintCallable, Category = "ExoNetwork|Write")
	static void WriteUint32(UPARAM(ref) TArray<uint8>& byteArray, int32 Value); // Int32 utilis� car BP n'a pas de uint32 natif

	UFUNCTION(BlueprintCallable, Category = "ExoNetwork|Write")
	static void WriteUint32At(UPARAM(ref) TArray<uint8>& byteArray, int32 Offset, int32 Value);

	UFUNCTION(BlueprintCallable, Category = "ExoNetwork|Write")
	static void WriteFloat(UPARAM(ref) TArray<uint8>& byteArray, float Value);

	UFUNCTION(BlueprintCallable, Category = "ExoNetwork|Write")
	static void WriteFloatAt(UPARAM(ref) TArray<uint8>& byteArray, int32 Offset, float Value);


	// --- STRING ---

	UFUNCTION(BlueprintCallable, Category = "ExoNetwork|Write")
	static void WriteString(UPARAM(ref) TArray<uint8>& byteArray, const FString& Value);

	UFUNCTION(BlueprintCallable, Category = "ExoNetwork|Write")
	static void WriteStringAt(UPARAM(ref) TArray<uint8>& byteArray, int32 Offset, const FString& Value);

	// --- Vector ---

	UFUNCTION(BlueprintCallable, Category = "ExoNetwork|Serialization")
	static void WriteVector(UPARAM(ref) TArray<uint8>& Bytes, const FVector& Value);

	UFUNCTION(BlueprintCallable, Category = "ExoNetwork|Serialization")
	static void WriteRotator(UPARAM(ref) TArray<uint8>& Bytes, const FRotator& Value);

	// ==============================================================================
	//                                   LECTURE (READ)
	// ==============================================================================

	static int32 ReadInt8(TArrayView<const uint8> ByteArrayView, UPARAM(ref) int32& Offset);
	static uint8 ReadUint8(TArrayView<const uint8> ByteArrayView, UPARAM(ref) int32& Offset);
	static int32 ReadInt16(TArrayView<const uint8> ByteArrayView, UPARAM(ref) int32& Offset);
	static int32 ReadUint16(TArrayView<const uint8> ByteArrayView, UPARAM(ref) int32& Offset);
	static int32 ReadInt32(TArrayView<const uint8> ByteArrayView, UPARAM(ref) int32& Offset);
	static int32 ReadUint32(TArrayView<const uint8> ByteArrayView, UPARAM(ref) int32& Offset);
	static float ReadFloat(TArrayView<const uint8> ByteArrayView, UPARAM(ref) int32& Offset);
	static FString ReadString(TArrayView<const uint8> ByteArrayView, UPARAM(ref) int32& Offset);
	static FVector ReadVector(TArrayView<const uint8> ByteArrayView, UPARAM(ref) int32& Offset);
	static FRotator ReadRotator(TArrayView<const uint8> ByteArrayView, UPARAM(ref) int32& Offset);
	
	static bool HasBytesLeft(TArrayView<const uint8> Bytes, int32 Offset, int32 SizeRequired);
	
	// Helper pour centraliser les Writes
	template <typename T>
	static void WriteStruct(TArray<uint8>& Bytes, const T& StructObj)
	{
		// Pour tous éléments d'une structure -> Serialize automatique sa propriété (VarType)
		WriteStructViaReflection(Bytes, T::StaticStruct(), &StructObj);
	}

	static void WriteStructViaReflection(TArray<uint8>& Bytes, const UScriptStruct* StructDefinition, const void* StructData);

	static void WriteProperty(TArray<uint8>& Bytes, FProperty* Property, const void* ValuePtr);

	template <typename T>
	static void ReadStruct(TArrayView<const uint8> Bytes, int32& Offset, T& OutStruct)
	{
		ReadStructViaReflection(Bytes, Offset, T::StaticStruct(), &OutStruct);
	}

	// La méthode interne lit chaque élément d'une struct
	static void ReadStructViaReflection(TArrayView<const uint8> Bytes, int32& Offset, const UScriptStruct* StructDefinition, void* StructData);

	// Le Switch Case variable type
	static void ReadProperty(TArrayView<const uint8> Bytes, int32& Offset, FProperty* Property, void* ValuePtr);
};