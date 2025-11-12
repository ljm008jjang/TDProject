// Copyright Epic Games, Inc. All Rights Reserved.

#include "TDProjectPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "TDProject.h"


ATDProjectPlayerController::ATDProjectPlayerController()
{
	// configure the controller
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
}

void ATDProjectPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	RotateToCursor();
}

void ATDProjectPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Only set up input on local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Context
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}

		// Set up action bindings
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this,
			                                   &ATDProjectPlayerController::OnMove);
		}
		else
		{
			UE_LOG(LogTDProject, Error,
			       TEXT(
				       "'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."
			       ), *GetNameSafe(this));
		}
	}
}

void ATDProjectPlayerController::OnMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	// add movement	
	GetPawn()->AddMovementInput(FVector::ForwardVector, MovementVector.Y);
	GetPawn()->AddMovementInput(FVector::RightVector, MovementVector.X);
}

void ATDProjectPlayerController::RotateToCursor()
{
	// 1. 마우스 커서 아래의 월드 위치 찾기
	FHitResult HitResult;
	// Visibility 채널을 기준으로 커서 아래에 있는 물체를 찾습니다.
	GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

	// 2. 부딪힌 지점이 유효하다면 (바닥 등을 클릭했다면)
	if (HitResult.bBlockingHit)
	{
		// 3. 컨트롤하는 폰(캐릭터) 가져오기
		APawn* ControlledPawn = GetPawn();
		if (ControlledPawn)
		{
			// 4. 캐릭터에서 마우스 위치까지의 방향 계산
			FVector TargetLocation = HitResult.Location;
			FVector PawnLocation = ControlledPawn->GetActorLocation();

			// 5. Z축(높이)은 무시하여 캐릭터가 기울어지지 않게 함
			FVector Direction = TargetLocation - PawnLocation;
			Direction.Z = 0.0f;

			// 6. 계산된 방향으로 새로운 회전값(Rotator) 생성
			FRotator NewRotation = Direction.Rotation();

			// 7. 캐릭터의 회전값을 즉시 변경
			// ControlledPawn->SetActorRotation(NewRotation);
			// Pawn의 회전을 직접 변경하는 대신, PlayerController의 ControlRotation을 변경합니다.
			// ControlRotation은 클라이언트에서 서버로 자동으로 복제됩니다.
			SetControlRotation(NewRotation);
		}
	}
}
