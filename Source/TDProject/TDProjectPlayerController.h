// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "TDProjectPlayerController.generated.h"

struct FInputActionValue;
class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  Player controller for a top-down perspective game.
 *  Implements point and click based controls
 */
UCLASS(abstract)
class ATDProjectPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	/** MappingContext */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;
public:

	/** Constructor */
	ATDProjectPlayerController();

protected:

	/** Initialize input bindings */
	virtual void SetupInputComponent() override;

private:
	/** Input handlers */
	void OnMove(const FInputActionValue& Value);

};


