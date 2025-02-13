// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SBGameInstance.generated.h"

/**
 *
 */
UCLASS()
class SPA7_PAWN_API USBGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	USBGameInstance();

	// ���� ��ü ���� ����
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameData")
	int32 TotalScore;
	// ���� ���� �ε��� (GameState������ ������ �� ������, �� ��ȯ �Ŀ��� �츮�� �ʹٸ� GameInstance�� ������ �� ����)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameData")
	int32 CurrentLevelIndex;

	UFUNCTION(BlueprintCallable, Category = "GameData")
	void AddToScore(int32 Amount);
};
