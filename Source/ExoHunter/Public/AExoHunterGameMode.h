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
	
	UFUNCTION(BlueprintImplementableEvent, Category = "ExoNetwork|Server")
	void BP_OnPlayerValidated(int32 PlayerID, const FClientConnectPacket& Packet);
	
	UFUNCTION(BlueprintCallable, Category = "ExoNetwork|Server")
	void BroadcastPlayerSpawned(const FServerSpawnActorPacket& SpawnPacket);
	
protected:
	UNet6ServerSubsystem* GetServerSubsystem() const;
};