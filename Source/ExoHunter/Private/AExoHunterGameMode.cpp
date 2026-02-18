// Fill out your copyright notice in the Description page of Project Settings.


#include "AExoHunterGameMode.h"
#include "UNet6ServerSubsystem.h" // On a besoin du Subsystem pour s'abonner
#include "Kismet/GameplayStatics.h"

void AExoHunterGameMode::BeginPlay()
{
    Super::BeginPlay();

    UGameInstance* GI = GetGameInstance();
    if (GI)
    {
        UNet6ServerSubsystem* ServerNet = GI->GetSubsystem<UNet6ServerSubsystem>();
        if (ServerNet)
        {
            ServerNet->OnNetworkEvent.AddDynamic(this, &AExoHunterGameMode::HandleUniversalEvent);
        }
    }
}

void AExoHunterGameMode::HandleUniversalEvent(EExoNetEventType Type, int32 PlayerId, const TArray<uint8>& Payload)
{
    switch (Type)
    {
    case EExoNetEventType::Connect:
        // On appelle l'événement Blueprint propre
        OnExoPlayerConnected(PlayerId);
        break;

    case EExoNetEventType::Disconnect:
        OnExoPlayerDisconnected(PlayerId);
        break;

    case EExoNetEventType::DataPacket:
        OnExoPacketReceived(PlayerId, Payload);
        break;
    }
}
