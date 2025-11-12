// Fill out your copyright notice in the Description page of Project Settings.


#include "FOWVisionComponent.h"

#include "FogOfWarInterface.h"
#include "Components/DecalComponent.h"
#include "Engine/Canvas.h"
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
	const APawn* OwningPawn = Cast<APawn>(GetOwner());

	if ((OwningPawn && OwningPawn->IsLocallyControlled()) == false)
	{
		// 로컬 플레이어가 아니면 데칼을 숨깁니다.
		DestroyComponent();
		DecalComponent->DestroyComponent();
		return;
	}


	if (DecalComponent && OwningPawn && OwningPawn->IsLocallyControlled())
	{
		// 1. 각 클라이언트마다 고유한 렌더 타겟을 동적으로 생성합니다.
		// 2. 템플릿의 모든 설정을 복사하여 고유한 렌더 타겟 인스턴스를 생성합니다.
		TextureRenderTarget2D = DuplicateObject<UTextureRenderTarget2D>(TextureRenderTarget2D, this);


		// 2. 데칼의 머티리얼에 대한 동적 인스턴스를 생성합니다.
		DecalMID = DecalComponent->CreateDynamicMaterialInstance();

		// 3. 생성된 DecalMID가 방금 만든 고유한 렌더 타겟을 참조하도록 설정합니다.
		//    (머티리얼에 'FOWTexture'라는 이름의 Texture2D 파라미터가 있어야 합니다)
		if (DecalMID && TextureRenderTarget2D)
		{
			DecalMID->SetTextureParameterValue(FName("FOWTexture"), TextureRenderTarget2D);
		}
	}

	SetTraceLineDistance(TraceLineDistance);
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
	if (DecalMID == nullptr)
	{
		return;
	}

	FName ParameterName = FName("PlayerPosition");
	FLinearColor NewValue = FLinearColor(GetComponentLocation());

	DecalMID->SetVectorParameterValue(ParameterName, NewValue);
}

void UFOWVisionComponent::CreateCone()
{
	TraceResults.Empty();
	UWorld* World = GetWorld();
	if (!World) { return; }

	FVector StartPoint = GetComponentLocation();

	// 이 컴포넌트를 소유한 액터의 현재 회전값을 가져옵니다.
	const FRotator ActorRotation = GetOwner()->GetActorRotation();

	// 이번 프레임에 감지된 액터들을 저장할 임시 세트
	TSet<AActor*> CurrentVisibleActors;

	// 360도를 N개의 트레이스로 나눌 때 각 트레이스 사이의 각도
	const float DegreePerTrace = 360.0f / NumTrace;

	// 전방 시야각의 절반 (중심을 기준으로 좌우를 검사하기 위함)
	const float HalfVisionConeAngle = VisionConeAngle * 0.5f;

	for (int i = 0; i < NumTrace; i++)
	{
		// 1. 현재 트레이스의 상대 각도 (0 ~ 360)
		const float CurrentRelativeAngle = i * DegreePerTrace;

		// 2. 현재 각도가 전방 시야각 내에 있는지 확인
		// 각도를 -180 ~ +180 범위로 변환하여 전방(0도)을 기준으로 쉽게 비교
		float AngleDelta = CurrentRelativeAngle;
		if (AngleDelta > 180.0f)
		{
			AngleDelta -= 360.0f; // 예: 350도 -> -10도
		}

		// AngleDelta의 절대값이 절반 시야각보다 작거나 같으면 전방 시야임
		bool bIsInVisionCone = FMath::Abs(AngleDelta) <= HalfVisionConeAngle;

		// 3. 조건에 맞는 트레이스 거리 선택
		float CurrentTraceDistance = bIsInVisionCone ? TraceLineDistance : TraceCircleDistance;

		// 4. 이 트레이스의 최종 월드 회전값 계산
		// (액터의 현재 회전값 + 이 트레이스의 상대 각도)
		FRotator RayRotation = ActorRotation;
		RayRotation.Yaw += CurrentRelativeAngle;

		// 5. 최종 방향 벡터 및 끝점 계산
		FVector RotatedVector = RayRotation.Vector(); // 정규화된 방향 벡터
		FVector EndPoint = StartPoint + RotatedVector * CurrentTraceDistance;

		// --- (이하 로직은 기존과 거의 동일) ---

		FHitResult HitResult;
		bool bBlockingHit = World->LineTraceSingleByChannel(HitResult, StartPoint, EndPoint, ECC_Visibility);

		FVector ActualEndPoint = EndPoint;
		FColor LineColor = bBlockingHit ? FColor::Red : FColor::Green;
		// bIsInVisionCone ? FColor::Green : FColor::Blue;

		if (bBlockingHit)
		{
			AActor* HitActor = HitResult.GetActor();
			if (HitActor && HitActor->Implements<UFogOfWarInterface>())
			{
				TraceResults.Add(EndPoint + RotatedVector * 10.0f); // 10.0f 오프셋
				CurrentVisibleActors.Add(HitActor);
			}
			else
			{
				TraceResults.Add(HitResult.Location + RotatedVector * 10.0f); // 10.0f 오프셋
				ActualEndPoint = HitResult.Location;
			}
		}
		else
		{
			TraceResults.Add(EndPoint + RotatedVector * 10.0f); // 10.0f 오프셋
		}

		//DrawDebugLine(World, StartPoint, ActualEndPoint, LineColor, false, 0.0f, 0, 1.0f);
	}

	// --- (액터 보임/숨김 처리는 기존과 동일) ---
	for (AActor* NewActor : CurrentVisibleActors)
	{
		if (!PreviouslyVisibleActors.Contains(NewActor))
		{
			IFogOfWarInterface::Execute_SetFOWHidden(NewActor, false); // 보이기
		}
	}

	for (AActor* OldActor : PreviouslyVisibleActors)
	{
		if (!CurrentVisibleActors.Contains(OldActor))
		{
			IFogOfWarInterface::Execute_SetFOWHidden(OldActor, true); // 숨기기
		}
	}

	PreviouslyVisibleActors = CurrentVisibleActors;
}

void UFOWVisionComponent::PrepareTrianglesForCanvas()
{
	CanvasTriangles.Empty();

	if (!TextureRenderTarget2D || TraceLineDistance <= 0.0f)
	{
		return;
	}

	// 삼각형의 중심점은 텍스처의 중앙입니다.
	const FVector2D CenterPos = TextureHalfSize;

	for (int i = 0; i < TraceResults.Num(); i++)
	{
		// 1. 월드 공간에서의 상대 위치 벡터를 구합니다.
		const FVector RelativePos1 = TraceResults[i] - GetComponentLocation();
		const FVector RelativePos2 = (i == TraceResults.Num() - 1)
			                             ? (TraceResults[0] - GetComponentLocation())
			                             : (TraceResults[i + 1] - GetComponentLocation());

		// 2. 상대 위치를 텍스처 공간에 맞게 스케일링하고 중심점을 더해 최종 2D 좌표를 계산합니다.
		const FVector2D V1Pos = FVector2D(RelativePos1.X, RelativePos1.Y) * WorldToTextureScale + CenterPos;
		const FVector2D V2Pos = FVector2D(RelativePos2.X, RelativePos2.Y) * WorldToTextureScale + CenterPos;

		FCanvasUVTri CanvasUVTri;
		CanvasUVTri.V0_Pos = CenterPos;
		CanvasUVTri.V1_Pos = V1Pos;
		CanvasUVTri.V2_Pos = V2Pos;
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

void UFOWVisionComponent::SetTraceLineDistance(int32 NewTraceLineDistance)
{
	TraceLineDistance = NewTraceLineDistance;
	if (DecalMID == nullptr || TextureRenderTarget2D == nullptr)
	{
		return;
	}

	TextureHalfSize = FVector2D(TextureRenderTarget2D->SizeX * 0.5f, TextureRenderTarget2D->SizeY * 0.5f);

	// 월드 좌표를 텍스처 좌표로 변환하기 위한 스케일 값입니다.
	// 가장 긴 탐지 거리가 텍스처의 절반 크기에 매핑되도록 합니다.
	// Clamp 문제로 인해 시야가 무한히 확장되는 것을 막기 위해, 텍스처 크기의 95%만 사용하도록 스케일링하여
	// 5%의 안전 여유 공간(검은색 테두리)을 남깁니다.
	const float SafeZoneScale = 0.95f;
	WorldToTextureScale = (TextureHalfSize.X * SafeZoneScale) / TraceLineDistance;

	FName ParameterName = FName("WorldToTextureScale");
	DecalMID->SetScalarParameterValue(ParameterName, WorldToTextureScale);
}
