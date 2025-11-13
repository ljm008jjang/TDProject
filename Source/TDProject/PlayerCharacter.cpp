// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "FOWVisionComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

APlayerCharacter::APlayerCharacter()
{
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	
	// Create the camera boom component
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));

	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false;

	// Create the camera component
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));

	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false;

	FOWVisionComponent = CreateDefaultSubobject<UFOWVisionComponent>(TEXT("FOWVision"));
	FOWVisionComponent->SetupAttachment(RootComponent);

	
	DecalComponent = CreateDefaultSubobject<UDecalComponent>("FOWDecal");
	DecalComponent->SetupAttachment(RootComponent);
	FOWVisionComponent->SetDecalComponent(DecalComponent);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocallyControlled() == false)
	{
		// 로컬 플레이어가 아니면 데칼을 숨깁니다.
		DecalComponent->DestroyComponent();
		FOWVisionComponent->DestroyComponent();
	}

	// stub
}

void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// stub
}
