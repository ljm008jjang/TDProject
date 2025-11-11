// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FogOfWarInterface.h"
#include "GameFramework/Character.h"
#include "TDProjectCharacter.generated.h"

/**
 *  A controllable top-down perspective character
 */
UCLASS(abstract)
class ATDProjectCharacter : public ACharacter, public IFogOfWarInterface
{
	GENERATED_BODY()

public:
	ATDProjectCharacter();


};

