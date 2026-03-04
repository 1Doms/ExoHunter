
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "BaseWorldSubsystem.generated.h"

UCLASS(Abstract, Blueprintable)
class EXOHUNTER_API UBaseWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
	protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "ExoHunter|Architecture")
	void ReceiveInitialize();
};
