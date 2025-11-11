// Fill out your copyright notice in the Description page of Project Settings.


#include "FOWVisionComponent.h"

#include "Components/DecalComponent.h"
#include "Engine/Canvas.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"

// Sets default values for this component's properties
UFOWVisionComponent::UFOWVisionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	DecalComponent = CreateDefaultSubobject<UDecalComponent>("FOWDecal");
	DecalComponent->SetupAttachment(this);
	// ...
}


// Called when the game starts
void UFOWVisionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UFOWVisionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SetMaterialParameterCollectionParameter();
	CreateCone();
	PrepareTrianglesForCanvas();
	DrawTriangles();
}

void UFOWVisionComponent::SetMaterialParameterCollectionParameter()
{
	if (MaterialParameterCollection == nullptr)
	{
		return;
	}

	FName ParameterName = FName("PlayerPosition");
	FLinearColor NewValue = FLinearColor(GetComponentLocation());

	UKismetMaterialLibrary::SetVectorParameterValue(this, MaterialParameterCollection, ParameterName, NewValue);
}

void UFOWVisionComponent::CreateCone()
{
	TraceResults.Empty();

	UWorld* World = GetWorld(); // 월드는 한 번만 가져옵니다.
	if (!World) { return; }

	FVector StartPoint = GetComponentLocation(); // 시작점은 동일합니다.
	FVector ForwardVec = GetForwardVector(); // 전방 벡터도 한 번만 가져옵니다.

	for (int i = 0; i < NumTrace; i++)
	{
		FVector MyVec = FVector(0, 0, NumTrace * DegreePerTrace * -0.5f + i * DegreePerTrace);
		FQuat MyQuat = FQuat::MakeFromEuler(MyVec);
		FVector RotatedVector = MyQuat.RotateVector(ForwardVec);

		FHitResult HitResult;
		FVector EndPoint = StartPoint + RotatedVector * TraceDistance;

		bool bBlockingHit = World->LineTraceSingleByChannel(HitResult, StartPoint, EndPoint, ECC_Visibility);

		// --- ↓↓↓ 디버그 라인 그리기 ↓↓↓ ---
		FVector ActualEndPoint = EndPoint; // 실제 라인의 끝점
		FColor LineColor = FColor::Green; // 기본색 (Miss)

		if (bBlockingHit)
		{
			TraceResults.Add(HitResult.Location + RotatedVector * 10.0f);

			ActualEndPoint = HitResult.Location; // Hit 지점까지
			LineColor = FColor::Red; // Hit 색상
		}
		else
		{
			TraceResults.Add(EndPoint + RotatedVector * 10.0f);
		}

		// 디버그 라인을 그립니다.
		DrawDebugLine(
			World,
			StartPoint, // 시작
			ActualEndPoint, // 끝 (Hit 지점 또는 EndPoint)
			LineColor, // 색상
			false, // PersistentLines (false = 한 프레임만)
			0.0f, // Lifetime (0.0f = 1 프레임)
			0, // DepthPriority
			1.0f // Thickness (두께)
		);
		// --- ↑↑↑ 디버그 라인 그리기 ↑↑↑ ---
	}
}

void UFOWVisionComponent::PrepareTrianglesForCanvas()
{
	CanvasTriangles.Empty();

	FVector TextureHalfSize = FVector(TextureRenderTarget2D->SizeX, TextureRenderTarget2D->SizeY, 0) * 0.5f;
	FVector V0Pos = TextureHalfSize;
	for (int i = 0; i < TraceResults.Num() - 2; i++)
	{
		FVector V1Pos = TraceResults[i] - GetComponentLocation() + TextureHalfSize;
		FVector V2Pos = TraceResults[i + 1] - GetComponentLocation() + TextureHalfSize;

		FCanvasUVTri CanvasUVTri;
		CanvasUVTri.V0_Pos = FVector2D(V0Pos);
		CanvasUVTri.V1_Pos = FVector2D(V1Pos);
		CanvasUVTri.V2_Pos = FVector2D(V2Pos);
		CanvasTriangles.Add(CanvasUVTri);
	}
}

void UFOWVisionComponent::DrawTriangles()
{
	if (TextureRenderTarget2D)
	{
		// 'this'는 WorldContextObject로 사용됩니다.
		UKismetRenderingLibrary::ClearRenderTarget2D(
			this, // WorldContextObject
			TextureRenderTarget2D, // 지울 렌더 타겟
			FLinearColor::Black // 지울 색상 (예: 검은색)
		);

		UCanvas* Canvas;
		FVector2D CanvasSize;
		FDrawToRenderTargetContext Context;

		UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(
			this, // WorldContextObject
			TextureRenderTarget2D, // 그릴 렌더 타겟
			Canvas, // [Out] 캔버스 객체
			CanvasSize, // [Out] 캔버스 크기
			Context // [Out] 컨텍스트
		);

		if (Canvas)
		{
			Canvas->K2_DrawTriangle(TextureRenderTarget2D, CanvasTriangles);

			// --- 3. 그리기 종료: 캔버스를 닫습니다 ---
		}
		UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(
			this, // WorldContextObject
			Context // 아까 받은 컨텍스트
		);
	}
}
