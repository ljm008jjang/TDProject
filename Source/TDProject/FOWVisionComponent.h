// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FOWVisionComponent.generated.h"

class UDecalComponent;
class UMaterialParameterCollection;
class TextureRenderTarget2D;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TDPROJECT_API UFOWVisionComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFOWVisionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UDecalComponent* DecalComponent;
	UPROPERTY(EditAnywhere, Category = "Material")
	UMaterialParameterCollection* MaterialParameterCollection;
	UPROPERTY(EditAnywhere, Category = "FOWParameter")
	int32 NumTrace;
	UPROPERTY(EditAnywhere, Category = "FOWParameter")
	float DegreePerTrace;
	UPROPERTY(EditAnywhere, Category = "FOWParameter")
	float TraceDistance;
	UPROPERTY()
	TArray<FVector> TraceResults;
	UPROPERTY()
	TArray<FCanvasUVTri> CanvasTriangles;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FOWParameter", meta = (AllowPrivateAccess = "true"))
	UTextureRenderTarget2D* TextureRenderTarget2D;

	void SetMaterialParameterCollectionParameter();
	void CreateCone();
	void PrepareTrianglesForCanvas();
	void DrawTriangles();

		
};
