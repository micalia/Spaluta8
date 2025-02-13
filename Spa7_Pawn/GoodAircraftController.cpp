#include "GoodAircraftController.h"
#include "EnhancedInputSubsystems.h"

AGoodAircraftController::AGoodAircraftController()
	: InputMappingContext(nullptr)
	, MoveAction(nullptr)
	, LookAction(nullptr)
{
}

void AGoodAircraftController::BeginPlay()
{
	Super::BeginPlay();

	// 현재 PlayerController에 연결된 LocalPlayer 객체를 가져옵니다.
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		// LocalPlayer에서 EnhancedInputLocalPlayerSubsystem을 획득합니다.
		if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			// 에디터에서 할당한 InputMappingContext가 있다면 활성화합니다.
			// 우선순위(Priority)는 0 (가장 높은 우선순위)로 설정합니다.
			if (InputMappingContext)
			{
				InputSubsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}