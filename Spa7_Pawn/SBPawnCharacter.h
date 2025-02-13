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

	// ���� ü���� �������� �Լ�
    UFUNCTION(BlueprintPure, Category = "Health")
    int32 GetHealth() const;
    // ü���� ȸ����Ű�� �Լ�
    UFUNCTION(BlueprintCallable, Category = "Health")
    void AddHealth(float Amount);
	void UpdateOverheadHP();
protected:

	// --- ������Ʈ --
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
	// --- �̵� ���� ���� ---
	// �ִ� �̵� �ӵ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MaxWalkSpeed = 900.f;

	// Pawn ȸ�� ���� (Yaw)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float TurnRate = 70.f;

	// ī�޶� ���� ȸ�� ���� (Pitch)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float LookUpRate = 70.f;

	// ī�޶� Pitch ���Ѱ� (�ּ�, �ִ�)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MinPitch = -80.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MaxPitch = 0.f;

	// �̵� �� �ӵ� ���� ��� (����/���� ȿ��)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float VelocityInterpSpeed = 10.f;

	// ���������� ����Ǵ� �̵� �ӵ� (AnimBP���� Ȱ��)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	FVector Velocity;

	// --- �Է� �� ���� ---
	FVector2D MovementInput; // �̵� �Է� (X: ����, Y: �¿�)
	FVector2D LookInput; // Look �Է� (X: Yaw, Y: Pitch)

protected:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// --- �Է� �ݹ� �Լ� ---
	void Move(const FInputActionValue& Value);
	void StopMove(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void StopLook(const FInputActionValue& Value);

	 // �ִ� ü��
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
    float MaxHealth;
    // ���� ü��
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Health")
    float Health;
    // ��� ó�� �Լ� (ü���� 0 ���ϰ� �Ǿ��� �� ȣ��)
    UFUNCTION(BlueprintCallable, Category = "Health")
    virtual void OnDeath();

		// ������ ó�� �Լ� - �ܺηκ��� �������� ���� �� ȣ���
    // �Ǵ� AActor�� TakeDamage()�� �������̵�
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

};
