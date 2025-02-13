#include "GoodAircraft.h"
#include "GoodAircraftController.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetMathLibrary.h"

AGoodAircraft::AGoodAircraft()
{
	PrimaryActorTick.bCanEverTick = true;

	// (1) �浹 �ݸ��� ������Ʈ ���� �� �ʱ�ȭ
	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComp"));
	CollisionComp->SetBoxExtent(FVector(50.f, 50.f, 50.f));
	CollisionComp->SetSimulatePhysics(false);
	RootComponent = CollisionComp;

	// (2) ��� �޽� ������Ʈ ���� �� �ݸ����� ����
	DroneMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DroneMeshComp"));
	DroneMeshComp->SetupAttachment(CollisionComp);
	DroneMeshComp->SetSimulatePhysics(false);

	// (3) ī�޶� ������Ʈ ���� �� �ݸ����� ����
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(CollisionComp);
	// ī�޶� ��ġ�� �������� ��ġ�Ͽ� �װ��� �Ĺ� ������ ����
	CameraComp->SetRelativeLocation(FVector(-300.f, 0.f, 100.f));

	// (4) �̵� ���� �⺻�� �ʱ�ȭ
	MaxSpeed = 1200.f;					// �ִ� �̵� �ӵ�
	Acceleration = 2000.f;			// ���ӵ�
	Deceleration = 800.f;				// ���ӵ�
	GravityZ = -500.f;					// �߷� ���ӵ� (Z��, ���� ��)
	AirFriction = 1.f;					// ���߿����� ���� ���
	GroundFriction = 8.f;				// ���鿡���� ���� ���
	Velocity = FVector::ZeroVector;		// ���� �̵� �ӵ� (���� ��ǥ��)
	bIsGrounded = false;				// ���� ���� (����� ���� ����)

	// (5) ȸ�� ���� �⺻�� �ʱ�ȭ
	PitchSpeed = 60.f;					// Pitch ȸ�� �ӵ�
	YawSpeed = 90.f;						// Yaw ȸ�� �ӵ�
	RollSpeed = 60.f;						// Roll ȸ�� �ӵ�
	MaxPitch = 80.f;						// �ִ� Pitch ����
	MaxRoll = 60.f;							// �ִ� Roll ����
	RotationRate = FRotator::ZeroRotator;	// �� ������ ������ ȸ�� �ӵ�

	// (6) �Է� ������ �⺻�� �ʱ�ȭ
	ForwardInput = 0.f;
	RightInput = 0.f;
	UpInput = 0.f;
}

void AGoodAircraft::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsGrounded) {
	GEngine->AddOnScreenDebugMessage(-1, 999, FColor::Cyan, FString::Printf(TEXT("%s >>Curr Velocity : %f Ground True"), *FDateTime::UtcNow().ToString(TEXT("%H:%M:%S")), Velocity.Length()), true, FVector2D(1.5f, 1.5f));
	}
	else {
	GEngine->AddOnScreenDebugMessage(-1, 999, FColor::Purple, FString::Printf(TEXT("%s >> Curr Velocity : %f Ground False"), *FDateTime::UtcNow().ToString(TEXT("%H:%M:%S")), Velocity.Length()), true, FVector2D(1.5f, 1.5f));

	}
	// �̵� �� ȸ�� ó�� �Լ� ȣ��
	HandleMovement(DeltaTime);
	HandleRotation(DeltaTime);
}

void AGoodAircraft::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// EnhancedInputComponent�� ĳ�����Ͽ� Enhanced Input �ý��� ���
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// ��Ʈ�ѷ��� GoodAircraftController�� ĳ�����Ͽ� �Է� �׼� ����
		if (AGoodAircraftController* MyController = Cast<AGoodAircraftController>(GetController()))
		{
			// --- MoveAction ���ε� (Axis3D: X=����, Y=�¿�, Z=����) ---
			if (MyController->MoveAction)
			{
				// �Է� �߻� �ÿ� �Է� ���� �� �� �� Move()�� ȣ���Ͽ� ���� 0���� �Ѿ �� �Է� ���� ó��
				EnhancedInput->BindAction(
					MyController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&AGoodAircraft::Move
				);
				EnhancedInput->BindAction(
					MyController->MoveAction,
					ETriggerEvent::Completed,
					this,
					&AGoodAircraft::Move
				);
			}

			// --- LookAction ���ε� (Axis3D: X=Roll, Y=Pitch, Z=Yaw) ---
			if (MyController->LookAction)
			{
				EnhancedInput->BindAction(
					MyController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&AGoodAircraft::Look
				);
				EnhancedInput->BindAction(
					MyController->LookAction,
					ETriggerEvent::Completed,
					this,
					&AGoodAircraft::Look
				);
			}
		}
	}
}

// Move �Է� �ݹ�.
// MoveAction�� Axis3D ���� �޾� ����(Forward), �¿�(Right), ����(Up) �Է��� ĳ��
void AGoodAircraft::Move(const FInputActionValue& Value)
{
	// �Է� �� (Axis3D: X=��/��, Y=��/��, Z=��/��)
	const FVector Axis3D = Value.Get<FVector>();

	// �� �� �Է��� ���� ������ ����
	ForwardInput = Axis3D.X;
	RightInput = Axis3D.Y;
	UpInput = Axis3D.Z;
}

// Look �Է� �ݹ�
// LookAction�� Axis3D ���� �޾� ȸ�� �ӵ� (Roll, Pitch, Yaw)�� ������Ʈ
void AGoodAircraft::Look(const FInputActionValue& Value)
{
	const FVector Axis3D = Value.Get<FVector>();

	// Roll: ���콺 ��, Pitch: ���콺 ����, Yaw: ���콺 �¿�
	RotationRate.Roll = Axis3D.X * RollSpeed;
	// ���콺 ���� �巡���ϸ� Pitch�� �����ϵ��� ���� ó��
	RotationRate.Pitch = -(Axis3D.Y) * PitchSpeed;
	RotationRate.Yaw = Axis3D.Z * YawSpeed;
}

// �̵� ó�� �Լ�
// �Է� ��, �߷�, ����/����, �ӵ� ����, ���� ���� �����Ͽ� ���� �ӵ��� ������Ʈ�ϰ�,
// ���� �ӵ��� Actor�� �̵���Ŵ.
void AGoodAircraft::HandleMovement(float DeltaTime)
{
	// (0) ����/���� ���¿� ���� �ִ� �ӵ��� ����/���� �� ���� (������Ʈ��)
	const bool bInAir = !bIsGrounded;
	const float ActualMaxSpeed = bInAir ? (MaxSpeed * 0.5f) : MaxSpeed;
	const float ActualAcceleration = bInAir ? (Acceleration * 0.5f) : Acceleration;
	const float ActualDeceleration = bInAir ? (Deceleration * 0.5f) : Deceleration;

	// (1) �߷� ����: ���鿡 ���� ������ Z�࿡ �߷� ���ӵ� ����
	if (!bIsGrounded)
	{
		Velocity.Z += GravityZ * DeltaTime;
	}

	// (2) �Է°����κ��� �̵� ���� ��� (���� ����)
	FVector LocalInput(ForwardInput, RightInput, UpInput);
	if (!LocalInput.IsNearlyZero())
	{
		LocalInput.Normalize();
	}
	// ���� Actor�� ȸ�� �������� �Է� ���͸� ���� �������� ��ȯ
	const FVector DesiredDirection = GetActorRotation().RotateVector(LocalInput);
	const FVector DesiredVelocity = DesiredDirection * ActualMaxSpeed;

	// (3) �Է� ������ ���� ���� �Ǵ� ���� ó��
	const bool bHasInput = !FMath::IsNearlyZero(ForwardInput) ||
		!FMath::IsNearlyZero(RightInput) ||
		!FMath::IsNearlyZero(UpInput);
	// �Է��� ������ ����, ������ ���� ����
	const float AccelFactor = bHasInput ? ActualAcceleration : ActualDeceleration;
	const FVector DeltaV = DesiredVelocity - Velocity;
	if (!DeltaV.IsNearlyZero())
	{
		const FVector AccelDir = DeltaV.GetSafeNormal();
		Velocity += (AccelDir * AccelFactor * DeltaTime);
	}

	// (4) �ӵ� ����: ����(XY)�� ����(Z) �ӵ��� ������ ����
	const float CurrentSpeedXY = FVector(Velocity.X, Velocity.Y, 0.f).Size();
	if (CurrentSpeedXY > ActualMaxSpeed)
	{
		const float RatioXY = ActualMaxSpeed / CurrentSpeedXY;
		Velocity.X *= RatioXY;
		Velocity.Y *= RatioXY;
	}
	const float CurrentSpeedZ = FMath::Abs(Velocity.Z);
	if (CurrentSpeedZ > ActualMaxSpeed)
	{
		const float RatioZ = ActualMaxSpeed / CurrentSpeedZ;
		Velocity.Z *= RatioZ;
	}

	// (5) ���� ����: ���� �ӵ��� ����Ͽ� ���� ó��
	const float Speed = Velocity.Size();
	const float CurrentFriction = bIsGrounded ? GroundFriction : AirFriction;
	if (Speed > KINDA_SMALL_NUMBER)
	{
		const float Drop = Speed * CurrentFriction * DeltaTime;
		const float NewSpeed = FMath::Max(Speed - Drop, 0.f);
		Velocity *= (NewSpeed / Speed);
	}

	// (6) �̵� ����: ������Ʈ�� Velocity�� Actor �̵�
	const FVector DeltaMove = Velocity * DeltaTime;
	FHitResult Hit;
	AddActorWorldOffset(DeltaMove, true, &Hit);

	// (7) �浹 ó��: ��ֹ��� �浹 �� ���� ���θ� �Ǵ�
	if (Hit.IsValidBlockingHit())
	{
		CheckGround(Hit);
	}
	else
	{
		bIsGrounded = false;
	}

	// (8) ���� ���¿��� �ϰ� �ӵ� ����: ���鿡 ������ Z�� �ϰ� �ӵ� 0���� ����
	if (bIsGrounded && Velocity.Z < 0.f)
	{
		Velocity.Z = 0.f;
	}
}

// ȸ�� ó�� �Լ�.
// �Էµ� ȸ�� �ӵ�(RotationRate)�� ���� ȸ�� ���� �ݿ��ϰ�, Pitch�� Roll�� ���� ������ ����
void AGoodAircraft::HandleRotation(float DeltaTime)
{
	// ���� ȸ������ �������� �Է� ȸ�� �ӵ� �ݿ�
	FRotator CurrentRot = GetActorRotation();
	CurrentRot.Roll += RotationRate.Roll * DeltaTime;
	CurrentRot.Pitch += RotationRate.Pitch * DeltaTime;
	CurrentRot.Yaw += RotationRate.Yaw * DeltaTime;

	// Pitch�� Roll ���� �����Ͽ� �ڿ������� ���� �ڼ� ����
	CurrentRot.Pitch = FMath::Clamp(CurrentRot.Pitch, -MaxPitch, MaxPitch);
	CurrentRot.Roll = FMath::Clamp(CurrentRot.Roll, -MaxRoll, MaxRoll);

	// ���� ȸ������ Actor�� ���� (�޽��� ī�޶� ��� ȸ��)
	SetActorRotation(CurrentRot);
}

// �浹 Hit ����� �������� ���� ���θ� üũ
// Hit.Normal.Z�� ���� �� �̻��̸� �������� ����
void AGoodAircraft::CheckGround(const FHitResult& Hit)
{
	// �Ϲ������� ������ ���� ���ʹ� ������ ���ϹǷ� Z ���� 0.7 �̻��̸� ������ �Ǵ�
	bIsGrounded = (Hit.Normal.Z > 0.7f);
}
