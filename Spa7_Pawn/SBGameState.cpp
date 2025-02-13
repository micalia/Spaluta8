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

	// 게임 시작 시 첫 레벨부터 진행
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
	// 시간이 다 되면 레벨을 종료
	//EndLevel(); >> 기존 코드
	OnGameOver();
}

void ASBGameState::OnCoinCollected()
{
	CollectedCoinCount++;

	UE_LOG(LogTemp, Warning, TEXT("Coin Collected: %d / %d"),
		CollectedCoinCount,
		SpawnedCoinCount)

		// 현재 레벨에서 스폰된 코인을 전부 주웠다면 즉시 레벨 종료
		if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
		{
			StartWave();
		}
}

void ASBGameState::EndLevel()
{
	// 타이머 해제
	GetWorldTimerManager().ClearTimer(LevelTimerHandle);

	// 다음 레벨 인덱스로
	if (USBGameInstance* GI = Cast<USBGameInstance>(GetGameInstance())) {
		GI->CurrentLevelIndex++;
		UE_LOG(LogTemp, Warning, TEXT("CurrentLevelIndex : %d"), GI->CurrentLevelIndex);
		// 모든 레벨을 다 돌았다면 게임 오버 처리
		if (GI->CurrentLevelIndex >= MaxLevels)
		{
			OnGameOver();
			return;
		}

		// 레벨 맵 이름이 있다면 해당 맵 불러오기
		if (LevelMapNames.IsValidIndex(GI->CurrentLevelIndex))
		{
			UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[GI->CurrentLevelIndex]);
		}
		else
		{
			// 맵 이름이 없으면 게임오버
			OnGameOver();
		}
	}
}

void ASBGameState::OnGameOver()
{
	// 여기서 UI를 띄운다거나, 재시작 기능을 넣을 수도 있음
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
		// TODO: 현재 웨이브가 웨이브 갯수보다 많다면 다음 웨이브로 전환
		EndLevel();
		return;
	}
	CleanItems(); // 기존에 이미 스폰된 아이템이 있다면 깔끔하게 전부 제거
	// 레벨 시작 시, 코인 개수 초기화
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;

	// 현재 맵에 배치된 모든 SpawnVolume을 찾아 아이템 40개를 스폰
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
					// 처음에는 무조건 코인 아이템 스폰
					SpawnedActor = SpawnVolume->DefaultCoinItemSpawn();
				}
				else {
					SpawnedActor = SpawnVolume->SpawnRandomItem();
				}
				// 만약 스폰된 액터가 코인 타입이라면 SpawnedCoinCount 증가
				if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
				{
					SpawnedCoinCount++;
				}
			}
		}
	}

	// 30초 후에 OnLevelTimeUp()가 호출되도록 타이머 설정
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
