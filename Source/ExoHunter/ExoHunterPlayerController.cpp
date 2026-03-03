// Copyright Epic Games, Inc. All Rights Reserved.


#include "ExoHunterPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ExoHunterTypes.h"
#include "UNet6ClientSubsystem.h"
#include "Engine/LocalPlayer.h"
#include "Kismet/GameplayStatics.h"

void AExoHunterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UGameInstance* GI = GetGameInstance())
	{
		CachedNetSubsystem = GI->GetSubsystem<UNet6ClientSubsystem>();
	}
	
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InputMappingContext, 0);
	}
}

void AExoHunterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EIC) return;

	// Déplacement ZQSD
	EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AExoHunterPlayerController::OnMove);
	EIC->BindAction(MoveAction, ETriggerEvent::Completed, this, &AExoHunterPlayerController::OnMoveCompleted);

#define BIND_BUTTON(Action, Flag) \
EIC->BindAction(Action, ETriggerEvent::Started, this, &AExoHunterPlayerController::HandleButtonInput, Flag, true); \
EIC->BindAction(Action, ETriggerEvent::Completed, this, &AExoHunterPlayerController::HandleButtonInput, Flag, false);

	BIND_BUTTON(SpaceAction,        EExoInputFlags::Space);
	BIND_BUTTON(LeftShiftAction,    EExoInputFlags::LeftShift);
	BIND_BUTTON(LeftControlAction,  EExoInputFlags::LeftControl);
	BIND_BUTTON(LeftClickAction,    EExoInputFlags::LeftClick);
	BIND_BUTTON(RightClickAction,   EExoInputFlags::RightClick);
	BIND_BUTTON(ReloadAction,       EExoInputFlags::Reload);
	BIND_BUTTON(PrimaryAction,      EExoInputFlags::Primary);
	BIND_BUTTON(SecondaryAction,    EExoInputFlags::Secondary);
	BIND_BUTTON(TertiaryAction,     EExoInputFlags::Tertiary);
	BIND_BUTTON(QuaternaryAction,   EExoInputFlags::quaternary);
	BIND_BUTTON(UltimateAction,     EExoInputFlags::Ultimate);

#undef BIND_BUTTON
}

// --- LOGIQUE DU DÉPLACEMENT (ZQSD) ---
void AExoHunterPlayerController::OnMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	// On nettoie les 4 bits de direction d'un coup
	CurrentFrameInput.CompressedInputs &= ~(uint16)(EExoInputFlags::Forward | EExoInputFlags::Backward | EExoInputFlags::Left | EExoInputFlags::Right);

	if (MovementVector.Y > 0.1f) CurrentFrameInput.CompressedInputs |= (uint16)EExoInputFlags::Forward;
	else if (MovementVector.Y < -0.1f) CurrentFrameInput.CompressedInputs |= (uint16)EExoInputFlags::Backward;

	if (MovementVector.X > 0.1f) CurrentFrameInput.CompressedInputs |= (uint16)EExoInputFlags::Right;
	else if (MovementVector.X < -0.1f) CurrentFrameInput.CompressedInputs |= (uint16)EExoInputFlags::Left;
}

void AExoHunterPlayerController::OnMoveCompleted(const FInputActionValue& Value)
{
	CurrentFrameInput.CompressedInputs &= ~(uint16)(EExoInputFlags::Forward | EExoInputFlags::Backward | EExoInputFlags::Left | EExoInputFlags::Right);
}

void AExoHunterPlayerController::HandleButtonInput(EExoInputFlags Flag, bool bPressed)
{
	if (bPressed) 
		CurrentFrameInput.CompressedInputs |= (uint16)Flag;
	else 
		CurrentFrameInput.CompressedInputs &= ~(uint16)Flag;
}

void AExoHunterPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!CachedNetSubsystem || !IsLocalPlayerController() || GetNetMode() != NM_Client) return;

	float ClampedDelta = FMath::Min(DeltaTime, 0.15f); 
	NetUpdateTimeBuffer += ClampedDelta;
    
	// On calcule la durée idéale d'un tick (ex: 1/60s = 0.0166s)
	float TargetInterval = 1.0f / FMath::Clamp(ServerTargetTickrate, 10.0f, 128.0f);

	// 2. BOUCLE D'ENVOI CONSTANTE (Régule le Jitter)
	while (NetUpdateTimeBuffer >= TargetInterval)
	{
		NetUpdateTimeBuffer -= TargetInterval;

		// On incrémente le numéro de séquence pour la Client-Side Prediction
		CurrentSequenceNumber++;
		CurrentFrameInput.SequenceNumber = CurrentSequenceNumber;
		CurrentFrameInput.InputDeltaTime = TargetInterval;

		// Capture de la caméra (elle doit être fluide donc on la lit au moment exact du tick)
		FRotator CameraRotation = GetControlRotation();
		CurrentFrameInput.CameraPitch = CameraRotation.Pitch;
		CurrentFrameInput.CameraYaw = CameraRotation.Yaw;

		// 3. SAUVEGARDE LOCALE (Pour la réconciliation CS)
		SavedMoves.Add(CurrentFrameInput);
		if (SavedMoves.Num() > MaxSavedMoves)
		{
			SavedMoves.RemoveAt(0); // On garde un buffer de taille fixe
		}

		// 4. ENVOI RÉSEAU
		CachedNetSubsystem->SendToServer(FInstancedStruct::Make(CurrentFrameInput), false);
	}
}