// Fill out your copyright notice in the Description page of Project Settings.

#include "AExoHunterGameState.h"
#include "UNet6ClientSubsystem.h"

UNet6ClientSubsystem* AExoHunterGameState::GetClientSubsystem() const
{
	if (UGameInstance* GI = GetGameInstance())
	{
		return GI->GetSubsystem<UNet6ClientSubsystem>();
	}
	return nullptr;
}
