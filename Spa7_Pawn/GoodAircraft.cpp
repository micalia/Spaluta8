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

	// (1) 충돌 콜리전 컴포넌트 생성 및 초기화
	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComp"));
	CollisionComp->SetBoxExtent(FVector(50.f, 50.f, 50.f));
	CollisionComp->SetSimulatePhysics(false);
	RootComponent = CollisionComp;

	// (2) 드론 메쉬 컴포넌트 생성 및 콜리전에 부착
	DroneMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DroneMeshComp"));
	DroneMeshComp->SetupAttachment(CollisionComp);
	DroneMeshComp->SetSimulatePhysics(false);

	// (3) 카메라 컴포넌트 생성 및 콜리전에 부착
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(CollisionComp);
	// 카메라 위치를 뒤쪽으로 배치하여 항공기 후방 시점을 제공
	CameraComp->SetRelativeLocation(FVector(-300.f, 0.f, 100.f));

	// (4) 이동 변수 기본값 초기화
	MaxSpeed = 1200.f;					// 최대 이동 속도
	Acceleration = 2000.f;			// 가속도
	Deceleration = 800.f;				// 감속도
	GravityZ = -500.f;					// 중력 가속도 (Z축, 음수 값)
	AirFriction = 1.f;					// 공중에서의 마찰 계수
	GroundFriction = 8.f;				// 지면에서의 마찰 계수
	Velocity = FVector::ZeroVector;		// 현재 이동 속도 (월드 좌표계)
	bIsGrounded = false;				// 착지 여부 (지면과 접촉 여부)

	// (5) 회전 변수 기본값 초기화
	PitchSpeed = 60.f;					// Pitch 회전 속도
	YawSpeed = 90.f;						// Yaw 회전 속도
	RollSpeed = 60.f;						// Roll 회전 속도
	MaxPitch = 80.f;						// 최대 Pitch 각도
	MaxRoll = 60.f;							// 최대 Roll 각도
	RotationRate = FRotator::ZeroRotator;	// 매 프레임 적용할 회전 속도

	// (6) 입력 변수들 기본값 초기화
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
	// 이동 및 회전 처리 함수 호출
	HandleMovement(DeltaTime);
	HandleRotation(DeltaTime);
}

void AGoodAircraft::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// EnhancedInputComponent로 캐스팅하여 Enhanced Input 시스템 사용
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// 컨트롤러를 GoodAircraftController로 캐스팅하여 입력 액션 참조
		if (AGoodAircraftController* MyController = Cast<AGoodAircraftController>(GetController()))
		{
			// --- MoveAction 바인딩 (Axis3D: X=전후, Y=좌우, Z=상하) ---
			if (MyController->MoveAction)
			{
				// 입력 발생 시와 입력 종료 시 둘 다 Move()를 호출하여 값이 0으로 넘어갈 때 입력 리셋 처리
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

			// --- LookAction 바인딩 (Axis3D: X=Roll, Y=Pitch, Z=Yaw) ---
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

// Move 입력 콜백.
// MoveAction의 Axis3D 값을 받아 전후(Forward), 좌우(Right), 상하(Up) 입력을 캐시
void AGoodAircraft::Move(const FInputActionValue& Value)
{
	// 입력 값 (Axis3D: X=전/후, Y=좌/우, Z=상/하)
	const FVector Axis3D = Value.Get<FVector>();

	// 각 축 입력을 개별 변수에 저장
	ForwardInput = Axis3D.X;
	RightInput = Axis3D.Y;
	UpInput = Axis3D.Z;
}

// Look 입력 콜백
// LookAction의 Axis3D 값을 받아 회전 속도 (Roll, Pitch, Yaw)를 업데이트
void AGoodAircraft::Look(const FInputActionValue& Value)
{
	const FVector Axis3D = Value.Get<FVector>();

	// Roll: 마우스 휠, Pitch: 마우스 상하, Yaw: 마우스 좌우
	RotationRate.Roll = Axis3D.X * RollSpeed;
	// 마우스 위로 드래그하면 Pitch가 감소하도록 음수 처리
	RotationRate.Pitch = -(Axis3D.Y) * PitchSpeed;
	RotationRate.Yaw = Axis3D.Z * YawSpeed;
}

// 이동 처리 함수
// 입력 값, 중력, 가속/감속, 속도 제한, 마찰 등을 적용하여 최종 속도를 업데이트하고,
// 계산된 속도로 Actor를 이동시킴.
void AGoodAircraft::HandleMovement(float DeltaTime)
{
	// (0) 지상/공중 상태에 따라 최대 속도와 가속/감속 값 조정 (에어컨트롤)
	const bool bInAir = !bIsGrounded;
	const float ActualMaxSpeed = bInAir ? (MaxSpeed * 0.5f) : MaxSpeed;
	const float ActualAcceleration = bInAir ? (Acceleration * 0.5f) : Acceleration;
	const float ActualDeceleration = bInAir ? (Deceleration * 0.5f) : Deceleration;

	// (1) 중력 적용: 지면에 있지 않으면 Z축에 중력 가속도 적용
	if (!bIsGrounded)
	{
		Velocity.Z += GravityZ * DeltaTime;
	}

	// (2) 입력값으로부터 이동 방향 계산 (로컬 공간)
	FVector LocalInput(ForwardInput, RightInput, UpInput);
	if (!LocalInput.IsNearlyZero())
	{
		LocalInput.Normalize();
	}
	// 현재 Actor의 회전 기준으로 입력 벡터를 월드 공간으로 변환
	const FVector DesiredDirection = GetActorRotation().RotateVector(LocalInput);
	const FVector DesiredVelocity = DesiredDirection * ActualMaxSpeed;

	// (3) 입력 유무에 따른 가속 또는 감속 처리
	const bool bHasInput = !FMath::IsNearlyZero(ForwardInput) ||
		!FMath::IsNearlyZero(RightInput) ||
		!FMath::IsNearlyZero(UpInput);
	// 입력이 있으면 가속, 없으면 감속 적용
	const float AccelFactor = bHasInput ? ActualAcceleration : ActualDeceleration;
	const FVector DeltaV = DesiredVelocity - Velocity;
	if (!DeltaV.IsNearlyZero())
	{
		const FVector AccelDir = DeltaV.GetSafeNormal();
		Velocity += (AccelDir * AccelFactor * DeltaTime);
	}

	// (4) 속도 제한: 수평(XY)과 수직(Z) 속도를 별도로 제한
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

	// (5) 마찰 적용: 현재 속도에 비례하여 감속 처리
	const float Speed = Velocity.Size();
	const float CurrentFriction = bIsGrounded ? GroundFriction : AirFriction;
	if (Speed > KINDA_SMALL_NUMBER)
	{
		const float Drop = Speed * CurrentFriction * DeltaTime;
		const float NewSpeed = FMath::Max(Speed - Drop, 0.f);
		Velocity *= (NewSpeed / Speed);
	}

	// (6) 이동 적용: 업데이트된 Velocity로 Actor 이동
	const FVector DeltaMove = Velocity * DeltaTime;
	FHitResult Hit;
	AddActorWorldOffset(DeltaMove, true, &Hit);

	// (7) 충돌 처리: 장애물과 충돌 시 지면 여부를 판단
	if (Hit.IsValidBlockingHit())
	{
		CheckGround(Hit);
	}
	else
	{
		bIsGrounded = false;
	}

	// (8) 착지 상태에서 하강 속도 보정: 지면에 있으면 Z축 하강 속도 0으로 유지
	if (bIsGrounded && Velocity.Z < 0.f)
	{
		Velocity.Z = 0.f;
	}
}

// 회전 처리 함수.
// 입력된 회전 속도(RotationRate)를 현재 회전 값에 반영하고, Pitch와 Roll에 대해 제한을 적용
void AGoodAircraft::HandleRotation(float DeltaTime)
{
	// 현재 회전값을 기준으로 입력 회전 속도 반영
	FRotator CurrentRot = GetActorRotation();
	CurrentRot.Roll += RotationRate.Roll * DeltaTime;
	CurrentRot.Pitch += RotationRate.Pitch * DeltaTime;
	CurrentRot.Yaw += RotationRate.Yaw * DeltaTime;

	// Pitch와 Roll 값을 제한하여 자연스러운 비행 자세 유지
	CurrentRot.Pitch = FMath::Clamp(CurrentRot.Pitch, -MaxPitch, MaxPitch);
	CurrentRot.Roll = FMath::Clamp(CurrentRot.Roll, -MaxRoll, MaxRoll);

	// 최종 회전값을 Actor에 적용 (메쉬와 카메라 모두 회전)
	SetActorRotation(CurrentRot);
}

// 충돌 Hit 결과를 바탕으로 지면 여부를 체크
// Hit.Normal.Z가 일정 값 이상이면 지면으로 간주
void AGoodAircraft::CheckGround(const FHitResult& Hit)
{
	// 일반적으로 지면의 법선 벡터는 위쪽을 향하므로 Z 값이 0.7 이상이면 착지로 판단
	bIsGrounded = (Hit.Normal.Z > 0.7f);
}
