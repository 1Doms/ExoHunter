// Copyright Epic Games, Inc. All Rights Reserved.

#include "ExoHunterGameMode.h"
#include "ExoHunterCharacter.h"
#include "UObject/ConstructorHelpers.h"

AExoHunterGameMode::AExoHunterGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
