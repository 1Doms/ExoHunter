#include "UBaseSubsystem.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h" // Pour récupérer le joueur facilement

void UBaseSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	ReceiveInitialize(); // Quand ceci s'appelle en BP, le monde est prêt ! Tu peux Create Widget.
}

UBaseSubsystem* UBaseSubsystem::GetManager(TSubclassOf<UBaseSubsystem> ManagerClass)
{
	if (!ManagerClass || !GetWorld()) return nullptr;

	// On utilise la librairie pour récupérer le World Subsystem
	if (USubsystem* FoundSub = USubsystemBlueprintLibrary::GetWorldSubsystem(this, ManagerClass))
	{
		return Cast<UBaseSubsystem>(FoundSub);
	}
	
	return nullptr;
}

APlayerController* UBaseSubsystem::GetExoPlayerController() const
{
	// Le World existe avec certitude ici, on chope le joueur local 0
	return UGameplayStatics::GetPlayerController(GetWorld(), 0);
}