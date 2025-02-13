// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SBPawnCharacter.generated.h"

class USkeletalMeshComponent;
class UCapsuleComponent;
class USpringArmComponent;
class UCameraComponent;
class UWidgetComponent;
struct FInputActionValue;

UCLASS()
class SPA7_PAWN_API ASBPawnCharacter : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASBPawnCharacter();

	virtual void BeginPlay() override;

	// 현재 체력을 가져오는 함수
    UFUNCTION(BlueprintPure, Category = "Health")
    int32 GetHealth() const;
    // 체력을 회복시키는 함수
    UFUNCTION(BlueprintCallable, Category = "Health")
    void AddHealth(float Amount);
	void UpdateOverheadHP();
protected:

	// --- 컴포넌트 --
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCapsuleComponent* CapsuleComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComp;

	UPROPERTY(EditAnywhere)
	UWidgetComponent* OverheadWidget;
	// --- 이동 관련 설정 ---
	// 최대 이동 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MaxWalkSpeed = 900.f;

	// Pawn 회전 감도 (Yaw)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float TurnRate = 70.f;

	// 카메라 상하 회전 감도 (Pitch)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float LookUpRate = 70.f;

	// 카메라 Pitch 제한값 (최소, 최대)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MinPitch = -80.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MaxPitch = 0.f;

	// 이동 시 속도 보간 계수 (가속/감속 효과)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float VelocityInterpSpeed = 10.f;

	// 최종적으로 적용되는 이동 속도 (AnimBP에서 활용)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	FVector Velocity;

	// --- 입력 값 저장 ---
	FVector2D MovementInput; // 이동 입력 (X: 전후, Y: 좌우)
	FVector2D LookInput; // Look 입력 (X: Yaw, Y: Pitch)

protected:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// --- 입력 콜백 함수 ---
	void Move(const FInputActionValue& Value);
	void StopMove(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void StopLook(const FInputActionValue& Value);

	 // 최대 체력
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
    float MaxHealth;
    // 현재 체력
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Health")
    float Health;
    // 사망 처리 함수 (체력이 0 이하가 되었을 때 호출)
    UFUNCTION(BlueprintCallable, Category = "Health")
    virtual void OnDeath();

		// 데미지 처리 함수 - 외부로부터 데미지를 받을 때 호출됨
    // 또는 AActor의 TakeDamage()를 오버라이드
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

};
