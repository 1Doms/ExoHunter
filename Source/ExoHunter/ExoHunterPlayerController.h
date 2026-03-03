// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PacketBuilder.h"
#include "GameFramework/PlayerController.h"
#include "ExoHunterPlayerController.generated.h"

class UInputMappingContext;

UCLASS()
class EXOHUNTER_API AExoHunterPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	/** Input Mapping Context */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* InputMappingContext;
	
	// --- LES ACTIONS (À configurer dans le Blueprint du PlayerController) ---
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Movement")
	UInputAction* MoveAction; // (Gère Forward, Backward, Left, Right via un Vector2D)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Actions")
	UInputAction* SpaceAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Actions")
	UInputAction* LeftShiftAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Actions")
	UInputAction* LeftControlAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Actions")
	UInputAction* LeftClickAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Actions")
	UInputAction* RightClickAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Actions")
	UInputAction* ReloadAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Abilities")
	UInputAction* PrimaryAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Abilities")
	UInputAction* SecondaryAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Abilities")
	UInputAction* TertiaryAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Abilities")
	UInputAction* QuaternaryAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Abilities")
	UInputAction* UltimateAction;

protected:
	FClientInputStruct CurrentFrameInput;
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaTime) override;

	// --- LES FONCTIONS DE CALLBACK ---
	
	// Déplacement (Axes)
	void OnMove(const FInputActionValue& Value);
	void OnMoveCompleted(const FInputActionValue& Value);

	void HandleButtonInput(EExoInputFlags Flag, bool bPressed);
	
private:
	UPROPERTY()
	class UNet6ClientSubsystem* CachedNetSubsystem;
	
	uint32 CurrentSequenceNumber = 0;
	
	TArray<FClientInputStruct> SavedMoves;
	const int32 MaxSavedMoves = 60;
	
	float NetUpdateTimeBuffer = 0.0f;
	float ServerTargetTickrate = 60.0f;
	FClientInputStruct LastSentInput;
};
