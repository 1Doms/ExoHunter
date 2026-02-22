#include "AExoHunterGameMode.h"
#include "UNet6ServerSubsystem.h"

UNet6ServerSubsystem* AExoHunterGameMode::GetServerSubsystem() const
{
	if (UGameInstance* GI = GetGameInstance())
	{
		return GI->GetSubsystem<UNet6ServerSubsystem>();
	}
	return nullptr;
}
