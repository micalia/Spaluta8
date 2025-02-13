// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SBGameState.generated.h"

/**
 *
 */
UCLASS()
class SPA7_PAWN_API ASBGameState : public AGameState
{
	GENERATED_BODY()

public:
	ASBGameState();

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, Category = "Score")
	int32 Score;
	// ���� �������� ������ ���� ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 SpawnedCoinCount;
	// �÷��̾ ������ ���� ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 CollectedCoinCount;
	// �� ������ �����Ǵ� �ð� (�� ����)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	float LevelDuration = 10.0f; // �� ������ 30��;
	// ���� ���� ���� ���� �ε���
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 CurrentLevelIndex;
	// ��ü ������ ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 MaxLevels;
	// ���� ���� �� �̸� �迭. ���� �ִ� �ε����� ���ʴ�� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TArray<FName> LevelMapNames;

	// �� ������ ������ ������ �ð��� �帣���� �����ϴ� Ÿ�̸�
	FTimerHandle LevelTimerHandle;
	FTimerHandle HUDUpdateTimerHandle;

	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetScore() const;
	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddScore(int32 Amount);
	// ������ ������ ������ �� (��� ���� ����) ����Ǵ� �Լ�
	UFUNCTION(BlueprintCallable, Category = "Level")
	void OnGameOver();

	// ������ ������ ��, ������ ���� �� Ÿ�̸� ����
	void StartLevel();
	// ���� ���� �ð��� ����Ǿ��� �� ȣ��
	void OnLevelTimeUp();
	// ������ �ֿ��� �� ȣ��
	void OnCoinCollected();
	// ������ ���� �����ϰ� ���� ������ �̵�
	void EndLevel();
	void UpdateHUD();

	void StartWave();
	void CleanItems();
public:
	UPROPERTY(EditAnywhere)
	TArray<int32> WaveSpawnItemCnt = {3,5,7};
	/*UPROPERTY(EditAnywhere)
	int32 Wave1SpawnItemCnt = 3;
	UPROPERTY(EditAnywhere)
	int32 Wave2SpawnItemCnt = 5;
	UPROPERTY(EditAnywhere)
	int32 Wave3SpawnItemCnt = 7;*/
private:
	int32 currWave = 0;
};
