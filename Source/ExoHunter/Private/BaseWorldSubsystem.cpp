#include "BaseWorldSubsystem.h"

void UBaseWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	ReceiveInitialize();
}
