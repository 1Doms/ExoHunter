// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AExoHunterGameMode.generated.h"

UCLASS()
class EXOHUNTER_API AExoHunterGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleUniversalEvent(EExoNetEventType Type, int32 PlayerId, const TArray<uint8>& Payload);

	// --- BLUEPRINT EVENT ---

	UFUNCTION(BlueprintImplementableEvent, Category = "ExoNetwork")
	void OnExoPlayerConnected(int32 PlayerId);

	UFUNCTION(BlueprintImplementableEvent, Category = "ExoNetwork")
	void OnExoPlayerDisconnected(int32 PlayerId);

	UFUNCTION(BlueprintImplementableEvent, Category = "ExoNetwork")
	void OnExoPacketReceived(int32 PlayerId, const TArray<uint8>& Data);
};
