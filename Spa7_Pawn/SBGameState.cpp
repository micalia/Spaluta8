// Fill out your copyright notice in the Description page of Project Settings.


#include "SBGameState.h"
#include "Kismet/GameplayStatics.h"
#include "SpawnVolume.h"
#include "CoinItem.h"
#include "SBGameInstance.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "SpartaPlayerController.h"

ASBGameState::ASBGameState()
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	CurrentLevelIndex = 0;
	MaxLevels = 3;
}

void ASBGameState::BeginPlay()
{
	Super::BeginPlay();

	// ���� ���� �� ù �������� ����
	StartLevel();

	GetWorldTimerManager().SetTimer(
		HUDUpdateTimerHandle,
		this,
		&ASBGameState::UpdateHUD,
		0.1f,
		true
	);
}

int32 ASBGameState::GetScore() const
{
	return Score;
}

void ASBGameState::AddScore(int32 Amount)
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USBGameInstance* SpartaGameInstance = Cast<USBGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			SpartaGameInstance->AddToScore(Amount);
		}
	}
}

void ASBGameState::StartLevel()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->ShowGameHUD();
		}
	}

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USBGameInstance* SpartaGameInstance = Cast<USBGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			CurrentLevelIndex = SpartaGameInstance->CurrentLevelIndex;
		}
	}

	StartWave();
}

void ASBGameState::OnLevelTimeUp()
{
	// �ð��� �� �Ǹ� ������ ����
	//EndLevel(); >> ���� �ڵ�
	OnGameOver();
}

void ASBGameState::OnCoinCollected()
{
	CollectedCoinCount++;

	UE_LOG(LogTemp, Warning, TEXT("Coin Collected: %d / %d"),
		CollectedCoinCount,
		SpawnedCoinCount)

		// ���� �������� ������ ������ ���� �ֿ��ٸ� ��� ���� ����
		if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
		{
			StartWave();
		}
}

void ASBGameState::EndLevel()
{
	// Ÿ�̸� ����
	GetWorldTimerManager().ClearTimer(LevelTimerHandle);

	// ���� ���� �ε�����
	if (USBGameInstance* GI = Cast<USBGameInstance>(GetGameInstance())) {
		GI->CurrentLevelIndex++;
		UE_LOG(LogTemp, Warning, TEXT("CurrentLevelIndex : %d"), GI->CurrentLevelIndex);
		// ��� ������ �� ���Ҵٸ� ���� ���� ó��
		if (GI->CurrentLevelIndex >= MaxLevels)
		{
			OnGameOver();
			return;
		}

		// ���� �� �̸��� �ִٸ� �ش� �� �ҷ�����
		if (LevelMapNames.IsValidIndex(GI->CurrentLevelIndex))
		{
			UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[GI->CurrentLevelIndex]);
		}
		else
		{
			// �� �̸��� ������ ���ӿ���
			OnGameOver();
		}
	}
}

void ASBGameState::OnGameOver()
{
	// ���⼭ UI�� ���ٰų�, ����� ����� ���� ���� ����
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->ShowMainMenu(true);
		}
	}
}

void ASBGameState::UpdateHUD()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController);
		{
			if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
			{
				if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("TimeTxt"))))
				{
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle);
					TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), RemainingTime)));
				}

				if (UGameInstance* GameInstance = GetGameInstance())
				{
					USBGameInstance* SpartaGameInstance = Cast<USBGameInstance>(GameInstance);
					if (SpartaGameInstance)
					{
						if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("ScoreTxt"))))
						{
							ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), SpartaGameInstance->TotalScore)));
						}

						if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("CurrLevelIdxTxt"))))
						{
							LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Level: %d"), SpartaGameInstance->CurrentLevelIndex + 1)));
						}

						if (UTextBlock* WaveText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("WaveTxt"))))
						{
							WaveText->SetText(FText::FromString(FString::Printf(TEXT("Wave: %d / %d"), currWave, WaveSpawnItemCnt.Num())));
						}

						if (UTextBlock* CoinText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("CoinTxt"))))
						{
							CoinText->SetText(FText::FromString(FString::Printf(TEXT("Coin : %d / %d"),	CollectedCoinCount, SpawnedCoinCount)));
						}
					}
				}
			}
		}
	}
}

void ASBGameState::StartWave()
{
	if (currWave >= WaveSpawnItemCnt.Num()) {
		// TODO: ���� ���̺갡 ���̺� �������� ���ٸ� ���� ���̺�� ��ȯ
		EndLevel();
		return;
	}
	CleanItems(); // ������ �̹� ������ �������� �ִٸ� ����ϰ� ���� ����
	// ���� ���� ��, ���� ���� �ʱ�ȭ
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;

	// ���� �ʿ� ��ġ�� ��� SpawnVolume�� ã�� ������ 40���� ����
	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	const int32 ItemToSpawn = WaveSpawnItemCnt[currWave++];

	for (int32 i = 0; i < ItemToSpawn; i++)
	{
		if (FoundVolumes.Num() > 0)
		{
			ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
			if (SpawnVolume)
			{
				AActor* SpawnedActor = nullptr;
				if (i == 0) {
					// ó������ ������ ���� ������ ����
					SpawnedActor = SpawnVolume->DefaultCoinItemSpawn();
				}
				else {
					SpawnedActor = SpawnVolume->SpawnRandomItem();
				}
				// ���� ������ ���Ͱ� ���� Ÿ���̶�� SpawnedCoinCount ����
				if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
				{
					SpawnedCoinCount++;
				}
			}
		}
	}

	// 30�� �Ŀ� OnLevelTimeUp()�� ȣ��ǵ��� Ÿ�̸� ����
	GetWorldTimerManager().SetTimer(
		LevelTimerHandle,
		this,
		&ASBGameState::OnLevelTimeUp,
		LevelDuration,
		false
	);

	UpdateHUD();
	if (FoundVolumes.Num() >= 1) {
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Purple, FString::Printf(TEXT("Start Wave%d !!"),currWave), true, FVector2D(1.5f, 1.5f));
	}
}

void ASBGameState::CleanItems()
{
	TArray<AActor*> WorldItems;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseItem::StaticClass(), WorldItems);

	for (auto& item : WorldItems)
	{
		item->Destroy();
	}
}
