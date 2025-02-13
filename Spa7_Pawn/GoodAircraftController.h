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

	// �����Ϳ��� ������ �Է� ���� ���ؽ�Ʈ (IMC)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputMappingContext* InputMappingContext;

	// �����Ϳ��� ������ Move �Է� �׼� (Axis3D: X=����, Y=�¿�, Z=����)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* MoveAction;

	// �����Ϳ��� ������ Look �Է� �׼� (Axis3D: X=Roll, Y=Pitch, Z=Yaw)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* LookAction;

protected:
	virtual void BeginPlay() override;
};
