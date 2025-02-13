// Fill out your copyright notice in the Description page of Project Settings.


#include "SBPawnCharacter.h"
#include "SpartaPlayerController.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"
#include "SBGameState.h"

ASBPawnCharacter::ASBPawnCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// 1) Capsule Component: ��Ʈ ������Ʈ�� ��� (�浹, �̵� ����)
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	RootComponent = CapsuleComp;
	CapsuleComp->InitCapsuleSize(34.f, 88.f);
	CapsuleComp->SetSimulatePhysics(false);

	// 2) Skeletal Mesh: Pawn�� ���� ǥ��
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(CapsuleComp);
	MeshComp->SetSimulatePhysics(false);

	// 3) SpringArm: ī�޶��� �Ÿ��� �����ϸ�, ȸ���� �����ϵ��� ����
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(CapsuleComp);
	SpringArmComp->TargetArmLength = 300.f;
	// Pawn�� ȸ���� ������ ī�޶� ���������� ȸ���ϵ��� ��
	SpringArmComp->bUsePawnControlRotation = false;

	// 4) Camera: ���� ȭ�鿡 ���� ī�޶�
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(MeshComp);
	OverheadWidget->SetWidgetSpace(EWidgetSpace::World);

	// �Է� �� �ʱ�ȭ
	MovementInput = FVector2D::ZeroVector;
	LookInput = FVector2D::ZeroVector;
	Velocity = FVector::ZeroVector;

	// �ʱ� ü�� ����
	MaxHealth = 100.0f;
	Health = MaxHealth;
}

void ASBPawnCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	UpdateOverheadHP();
}

int32 ASBPawnCharacter::GetHealth() const
{
	return Health;
}

void ASBPawnCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 1) ī�޶��� ���� ȸ�� ������ Yaw�� ����
	const FRotator CameraRotation = CameraComp->GetComponentRotation();
	const FRotator YawOnlyRotation(0.f, CameraRotation.Yaw, 0.f);

	// 2) ī�޶� �������� Forward, Right ���� ���
	// Unreal ����: X = Forward, Y = Right, Z = Up
	const FVector CameraForward = FRotationMatrix(YawOnlyRotation).GetUnitAxis(EAxis::X);
	const FVector CameraRight = FRotationMatrix(YawOnlyRotation).GetUnitAxis(EAxis::Y);

	// 3) �Է°��� ���� �̵� ���� ����
	//    MovementInput.X: ���� (W/S), MovementInput.Y: �¿� (A/D)
	const FVector InputDirection = (CameraForward * MovementInput.X) + (CameraRight * MovementInput.Y);

	// 4) ��ǥ �̵� �ӵ� ��� (����ȭ�Ͽ� �ִ� �ӵ� ����)
	const FVector DesiredVelocity = InputDirection.GetSafeNormal() * MaxWalkSpeed;

	// 5) ���� �ӵ��� ����(interpolation) �����Ͽ� �ε巯�� ������ ����
	Velocity = FMath::VInterpTo(Velocity, DesiredVelocity, DeltaTime, VelocityInterpSpeed);

	// 6) ���� ��ǥ�迡�� �̵� ó�� (�浹 üũ ����)
	AddActorWorldOffset(Velocity * DeltaTime, /*bSweep=*/false);

	// 7) Pawn ��ü ȸ�� ó�� (���콺 �¿� �Է� ���)
	if (!FMath::IsNearlyZero(LookInput.X))
	{
		const float YawRotation = LookInput.X * TurnRate * DeltaTime;
		AddActorLocalRotation(FRotator(0.f, YawRotation, 0.f));
	}

	// 8) �������� ȸ�� ó�� (���콺 ���� �Է� ���: ī�޶� Pitch ����)
	if (!FMath::IsNearlyZero(LookInput.Y))
	{
		// ���� ���������� ���� ȸ������ �����´�.
		FRotator CurrentArmRotation = SpringArmComp->GetRelativeRotation();
		// ���콺 �Է¿� ���� Pitch ���� (LookUpRate ����)
		float NewPitch = CurrentArmRotation.Pitch - (LookInput.Y * LookUpRate * DeltaTime);
		// Pitch ���� �ּ�/�ִ� ������ ����
		NewPitch = FMath::Clamp(NewPitch, MinPitch, MaxPitch);
		CurrentArmRotation.Pitch = NewPitch;
		SpringArmComp->SetRelativeRotation(CurrentArmRotation);
	}
}

// �Է� ���ε�
void ASBPawnCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// EnhancedInputComponent�� ĳ�����Ͽ� Enhanced Input �ý��� ���
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// ���� ��Ʈ�ѷ��� SpartaPlayerController���� Ȯ�� �� �Է� �׼��� ������
		if (ASpartaPlayerController* MyController = Cast<ASpartaPlayerController>(GetController()))
		{
			// --- MoveAction ���ε� ---
			if (MyController->MoveAction)
			{
				// �Է� �߻� �� Move() ȣ��
				EnhancedInput->BindAction(
					MyController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&ASBPawnCharacter::Move
				);
				// �Է� ���� �� StopMove() ȣ���Ͽ� �̵� �Է� ����
				EnhancedInput->BindAction(
					MyController->MoveAction,
					ETriggerEvent::Completed,
					this,
					&ASBPawnCharacter::StopMove
				);
			}

			// --- LookAction ���ε� ---
			if (MyController->LookAction)
			{
				// �Է� �߻� �� Look() ȣ��
				EnhancedInput->BindAction(
					MyController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&ASBPawnCharacter::Look
				);
				// �Է� ���� �� StopLook() ȣ���Ͽ� Look �Է� ����
				EnhancedInput->BindAction(
					MyController->LookAction,
					ETriggerEvent::Completed,
					this,
					&ASBPawnCharacter::StopLook
				);
			}
		}
	}
}

// �Է� �ݹ� �Լ�
// Move �׼� ó��: 2D ���� �Է� ����
void ASBPawnCharacter::Move(const FInputActionValue& Value)
{
	MovementInput = Value.Get<FVector2D>();
}

// Move �Է� ����: �Է°��� 0���� ����
void ASBPawnCharacter::StopMove(const FInputActionValue& Value)
{
	MovementInput = FVector2D::ZeroVector;
}

// Look �׼� ó��: 2D ���� �Է� ���� (Yaw, Pitch)
void ASBPawnCharacter::Look(const FInputActionValue& Value)
{
	LookInput = Value.Get<FVector2D>();
}

// Look �Է� ����: �Է°��� 0���� ����
void ASBPawnCharacter::StopLook(const FInputActionValue& Value)
{
	LookInput = FVector2D::ZeroVector;
}

// ü�� ȸ�� �Լ�
void ASBPawnCharacter::AddHealth(float Amount)
{
	// ü���� ȸ����Ŵ. �ִ� ü���� �ʰ����� �ʵ��� ������
	Health = FMath::Clamp(Health + Amount, 0.0f, MaxHealth);
	UE_LOG(LogTemp, Log, TEXT("Health increased to: %f"), Health);
}

// ������ ó�� �Լ�
float ASBPawnCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// �⺻ ������ ó�� ���� ȣ�� (�ʼ��� �ƴ�)
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// ü���� ��������ŭ ���ҽ�Ű��, 0 ���Ϸ� �������� �ʵ��� Clamp
	Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);
	UpdateOverheadHP();

	UE_LOG(LogTemp, Warning, TEXT("Health decreased to: %f"), Health);

	// ü���� 0 ���ϰ� �Ǹ� ��� ó��
	if (Health <= 0.0f)
	{
		OnDeath();
	}

	// ���� ����� �������� ��ȯ
	return ActualDamage;
}

// ��� ó�� �Լ�
void ASBPawnCharacter::OnDeath()
{
	ASBGameState* SpartaGameState = GetWorld() ? GetWorld()->GetGameState<ASBGameState>() : nullptr;
	if (SpartaGameState)
	{
		SpartaGameState->OnGameOver();
	}
}

void ASBPawnCharacter::UpdateOverheadHP()
{
	if (!OverheadWidget) return;

	UUserWidget* OverheadWidgetInstance = OverheadWidget->GetUserWidgetObject();
	if (!OverheadWidgetInstance) return;

	if (UTextBlock* HPText = Cast<UTextBlock>(OverheadWidgetInstance->GetWidgetFromName(TEXT("OverHeadHP"))))
	{
		HPText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), Health, MaxHealth)));
	}
}