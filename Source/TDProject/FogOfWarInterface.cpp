// Fill out your copyright notice in the Description page of Project Settings.


#include "FogOfWarInterface.h"

// Add default functionality here for any IFogOfWarInterface functions that are not pure virtual.
void IFogOfWarInterface::SetFOWHidden_Implementation(bool bIsHidden)
{
	/*AActor* OwnerActor = dynamic_cast<AActor*>(this);

	if (OwnerActor)
	{
		// 2. 성공하면 OwnerActor의 기능(예: GetWorld)을 마음껏 사용할 수 있습니다.
		UE_LOG(LogTemp, Warning, TEXT("'%s' 액터의 SetFOWHidden 기본 구현이 호출됨!"), 
			*OwnerActor->GetName());

		// 예: 이 액터의 실제 Owner(스포너 등)를 가져올 수도 있습니다.
		AActor* ActualOwner = OwnerActor->GetOwner();
	}
	else
	{
		// 만약 AActor가 아닌 UObject (예: UGameInstance)가 구현했다면?
		UObject* OwnerObject = dynamic_cast<UObject*>(this);
		if (OwnerObject)
		{
			UE_LOG(LogTemp, Warning, TEXT("'%s' 오브젝트의 SetFOWHidden 기본 구현이 호출됨!"),
				*OwnerObject->GetName());
		}
	}*/
}