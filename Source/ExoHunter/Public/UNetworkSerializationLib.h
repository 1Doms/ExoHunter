#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UnetworkSerializationLib.generated.h"

/**
 * UNetworkSerializationLib
 * * Bibliothèque statique pour la sérialisation binaire (Raw Bytes).
 * Permet de convertir les types Unreal (High Level) en paquets réseau (Low Level).
 * * OPTIMISATIONS :
 * - Les FVector (Double precision dans UE5) sont convertis en Float (Simple precision) pour gagner 50% de place.
 * - Les FRotator sont convertis en Float (ou Short compressé dans une version future).
 */

UCLASS()
class EXOHUNTER_API UNetworkSerializationLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	// ==============================================================================
	//                                   ÉCRITURE (WRITE)
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
	static void WriteUint32(UPARAM(ref) TArray<uint8>& byteArray, int32 Value); // Int32 utilisé car BP n'a pas de uint32 natif

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

	UFUNCTION(BlueprintPure, Category = "ExoNetwork|Read")
	static int32 ReadInt8(const TArray<uint8>& byteArray, UPARAM(ref) int32& Offset);

	UFUNCTION(BlueprintPure, Category = "ExoNetwork|Read")
	static uint8 ReadUint8(const TArray<uint8>& byteArray, UPARAM(ref) int32& Offset);

	UFUNCTION(BlueprintPure, Category = "ExoNetwork|Read")
	static int32 ReadInt16(const TArray<uint8>& byteArray, UPARAM(ref) int32& Offset);

	UFUNCTION(BlueprintPure, Category = "ExoNetwork|Read")
	static int32 ReadUint16(const TArray<uint8>& byteArray, UPARAM(ref) int32& Offset);

	UFUNCTION(BlueprintPure, Category = "ExoNetwork|Read")
	static int32 ReadInt32(const TArray<uint8>& byteArray, UPARAM(ref) int32& Offset);

	UFUNCTION(BlueprintPure, Category = "ExoNetwork|Read")
	static int32 ReadUint32(const TArray<uint8>& byteArray, UPARAM(ref) int32& Offset);

	UFUNCTION(BlueprintPure, Category = "ExoNetwork|Read")
	static float ReadFloat(const TArray<uint8>& byteArray, UPARAM(ref) int32& Offset);

	UFUNCTION(BlueprintPure, Category = "ExoNetwork|Read")
	static FString ReadString(const TArray<uint8>& byteArray, UPARAM(ref) int32& Offset);

	UFUNCTION(BlueprintPure, Category = "ExoNetwork|Serialization")
	static FVector ReadVector(const TArray<uint8>& Bytes, UPARAM(ref) int32& Offset);

	UFUNCTION(BlueprintPure, Category = "ExoNetwork|Serialization")
	static FRotator ReadRotator(const TArray<uint8>& Bytes, UPARAM(ref) int32& Offset);

	// Helper pour vérifier la taille dispo
	static bool HasBytesLeft(const TArray<uint8>& byteArray, int32 Offset, int32 SizeRequired);

	static void WriteStructViaReflection(TArray<uint8>& Bytes, const UScriptStruct* StructDefinition, const void* StructData);

	template <typename T>
	static void WriteStruct(TArray<uint8>& Bytes, const T& StructObj)
	{
		WriteStructViaReflection(Bytes, T::StaticStruct(), &StructObj);
	}

	static void WriteProperty(TArray<uint8>& Bytes, FProperty* Property, const void* ValuePtr);
};