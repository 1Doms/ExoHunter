#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "UBaseSubsystem.generated.h"

UCLASS(Abstract, Blueprintable)
class EXOHUNTER_API UBaseSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "ExoHunter|Architecture")
	void OnHandleRequest(FName RequestID, UObject* Payload);

	// Le "Get Manager" magique reste le même !
	UFUNCTION(BlueprintPure, Category = "ExoHunter|Architecture", meta = (DeterminesOutputType = "ManagerClass"))
	UBaseSubsystem* GetManager(TSubclassOf<UBaseSubsystem> ManagerClass);

	// Plus besoin de GetWorld(), on crée juste un raccourci très sûr pour le Player
	UFUNCTION(BlueprintPure, Category = "ExoHunter|System")
	APlayerController* GetExoPlayerController() const;
	
protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "ExoHunter|Architecture")
	void ReceiveInitialize();
};