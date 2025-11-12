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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FOWParameter", meta = (AllowPrivateAccess = "true"))
	UDecalComponent* DecalComponent;
	UPROPERTY(EditAnywhere, Category = "FOWParameter")
	UMaterialParameterCollection* MaterialParameterCollection;
	UPROPERTY(Transient) // 저장할 필요 없음
	UMaterialInstanceDynamic* DecalMID;
	UPROPERTY(EditAnywhere, Category = "FOWParameter")
	int32 NumTrace = 60;
	UPROPERTY(EditAnywhere, Category = "FOWParameter")
	float TraceLineDistance = 1500.0f;
	/** 전방 시야각을 제외한 360도 원형 영역에서 사용할 짧은 트레이스 거리 */
	UPROPERTY(EditAnywhere, Category = "FOWParameter")
	float TraceCircleDistance = 500.0f;
	/** 전방 시야각의 총 각도 (예: 90도) */
	UPROPERTY(EditAnywhere, Category = "FOWParameter", meta = (UIMin = "0.0", UIMax = "360.0"))
	float VisionConeAngle = 90.0f;
	UPROPERTY()
	TArray<FVector> TraceResults;
	UPROPERTY()
	TArray<FCanvasUVTri> CanvasTriangles;
	/** 각 클라이언트별로 동적으로 생성되는 렌더 타겟입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FOWParameter", meta = (AllowPrivateAccess = "true"))
	UTextureRenderTarget2D* TextureRenderTarget2D;
	UPROPERTY()
	TSet<AActor*> PreviouslyVisibleActors;
	float WorldToTextureScale;
	FVector2D TextureHalfSize;

	void SetMaterialParameterCollectionParameter();
	void CreateCone();
	void PrepareTrianglesForCanvas();
	void DrawTriangles();
	//void DrawTriangles();

public:
	void SetTraceLineDistance(int32 NewTraceLineDistance);
		
};
