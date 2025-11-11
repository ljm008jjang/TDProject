// Fill out your copyright notice in the Description page of Project Settings.


#include "FogOfWar.h"

// Sets default values
AFogOfWar::AFogOfWar()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>("DefaultSceneRoot");
	PlaneMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Plane"));
	PlaneMeshComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AFogOfWar::BeginPlay()
{
	Super::BeginPlay();
	
	if (PlaneMeshComponent == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("No PlaneMeshComponent!"));
		return;
	}

	MIDFogOfWar = PlaneMeshComponent->CreateDynamicMaterialInstance(0);
	PlaneMeshComponent->SetMaterial(0, MIDFogOfWar);
}

// Called every frame
void AFogOfWar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFogOfWar::UpdateCutoutLocation_Implementation(FVector Vector)
{
	if (MIDFogOfWar == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("No MIDFogOfWar!"));
		return;
	}
	MIDFogOfWar->SetVectorParameterValue(FName("Location"), Vector);
}
