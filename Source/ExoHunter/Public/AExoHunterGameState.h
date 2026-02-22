// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "AExoHunterGameState.generated.h"

class UNet6ClientSubsystem;

UCLASS()
class EXOHUNTER_API AExoHunterGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "ExoNetwork|Server")
	void BP_OnSpawnActor(const FServerSpawnActorStruct& Struct);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "ExoNetwork|Server")
	void BP_OnClientConnecting(const FServerClientConnectingStruct& Struct);
	
protected:
	UNet6ClientSubsystem* GetClientSubsystem() const;
};
