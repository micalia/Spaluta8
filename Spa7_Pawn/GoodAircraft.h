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
	// --- ������Ʈ ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* DroneMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComp;

	// --- �̵� ������ ---
	// �ִ� �̵� �ӵ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	float MaxSpeed = 1200.f;

	// ���ӵ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	float Acceleration = 2000.f;

	// ���ӵ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	float Deceleration = 800.f;

	// �߷� ���ӵ� (Z��, ���� ��)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight|Gravity")
	float GravityZ = -500.f;

	// ���߿����� ���� ���
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight|Friction")
	float AirFriction = 1.f;

	// ���鿡���� ���� ���
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight|Friction")
	float GroundFriction = 8.f;

	// ���� �̵� �ӵ� (���� ��ǥ��)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flight")
	FVector Velocity;

	// ���� ���� (����� ���� ����)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flight")
	bool bIsGrounded;

	// --- ȸ�� ������ ---
	// Pitch ȸ�� �ӵ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	float PitchSpeed;

	// Yaw ȸ�� �ӵ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	float YawSpeed;

	// Roll ȸ�� �ӵ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	float RollSpeed;

	// �ִ� Pitch ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	float MaxPitch;

	// �ִ� Roll ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	float MaxRoll;

	// �� ������ ������ ȸ�� �ӵ�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flight")
	FRotator RotationRate;

	// --- Input Actions ---
	// Move �Է� �׼� (Axis3D: X=����, Y=�¿�, Z=����)
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* MoveAction;

	// Look �Է� �׼� (Axis3D: X=Roll, Y=Pitch, Z=Yaw)
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* LookAction;

	// ���� �Է� ���� ����
	float ForwardInput;
	float RightInput;
	float UpInput;

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// �̵� �� ȸ�� ó���� ���� �Լ�
	void HandleMovement(float DeltaTime);
	void HandleRotation(float DeltaTime);
	void CheckGround(const FHitResult& Hit);
	// �Է� �ݹ� �Լ�
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

};
