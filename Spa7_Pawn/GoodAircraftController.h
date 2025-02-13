// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GoodAircraftController.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class SPA7_PAWN_API AGoodAircraftController : public APlayerController
{
	GENERATED_BODY()
public:
	AGoodAircraftController();

	// 에디터에서 설정할 입력 매핑 컨텍스트 (IMC)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputMappingContext* InputMappingContext;

	// 에디터에서 설정할 Move 입력 액션 (Axis3D: X=전후, Y=좌우, Z=상하)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* MoveAction;

	// 에디터에서 설정할 Look 입력 액션 (Axis3D: X=Roll, Y=Pitch, Z=Yaw)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* LookAction;

protected:
	virtual void BeginPlay() override;
};
