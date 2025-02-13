// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "GoodAircraft.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UCameraComponent;
class UInputAction;

UCLASS()
class SPA7_PAWN_API AGoodAircraft : public APawn
{
	GENERATED_BODY()

public:
	AGoodAircraft();

protected:
	// --- 컴포넌트 ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* DroneMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComp;

	// --- 이동 변수들 ---
	// 최대 이동 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	float MaxSpeed = 1200.f;

	// 가속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	float Acceleration = 2000.f;

	// 감속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	float Deceleration = 800.f;

	// 중력 가속도 (Z축, 음수 값)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight|Gravity")
	float GravityZ = -500.f;

	// 공중에서의 마찰 계수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight|Friction")
	float AirFriction = 1.f;

	// 지면에서의 마찰 계수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight|Friction")
	float GroundFriction = 8.f;

	// 현재 이동 속도 (월드 좌표계)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flight")
	FVector Velocity;

	// 착지 여부 (지면과 접촉 여부)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flight")
	bool bIsGrounded;

	// --- 회전 변수들 ---
	// Pitch 회전 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	float PitchSpeed;

	// Yaw 회전 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	float YawSpeed;

	// Roll 회전 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	float RollSpeed;

	// 최대 Pitch 각도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	float MaxPitch;

	// 최대 Roll 각도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	float MaxRoll;

	// 매 프레임 적용할 회전 속도
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flight")
	FRotator RotationRate;

	// --- Input Actions ---
	// Move 입력 액션 (Axis3D: X=전후, Y=좌우, Z=상하)
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* MoveAction;

	// Look 입력 액션 (Axis3D: X=Roll, Y=Pitch, Z=Yaw)
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* LookAction;

	// 내부 입력 저장 변수
	float ForwardInput;
	float RightInput;
	float UpInput;

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// 이동 및 회전 처리를 위한 함수
	void HandleMovement(float DeltaTime);
	void HandleRotation(float DeltaTime);
	void CheckGround(const FHitResult& Hit);
	// 입력 콜백 함수
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

};
