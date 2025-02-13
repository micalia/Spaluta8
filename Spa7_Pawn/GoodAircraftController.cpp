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

	// ���� PlayerController�� ����� LocalPlayer ��ü�� �����ɴϴ�.
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		// LocalPlayer���� EnhancedInputLocalPlayerSubsystem�� ȹ���մϴ�.
		if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			// �����Ϳ��� �Ҵ��� InputMappingContext�� �ִٸ� Ȱ��ȭ�մϴ�.
			// �켱����(Priority)�� 0 (���� ���� �켱����)�� �����մϴ�.
			if (InputMappingContext)
			{
				InputSubsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}