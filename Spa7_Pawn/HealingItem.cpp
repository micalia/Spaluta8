// Fill out your copyright notice in the Description page of Project Settings.

#include "SBPawnCharacter.h"
#include "HealingItem.h"

AHealingItem::AHealingItem()
{
	HealAmount = 20.0f;
	ItemType = "Healing";
}

void AHealingItem::ActivateItem(AActor* Activator)
{
    if (Activator && Activator->ActorHasTag("Player"))
    {
        // ȸ�� ����� �޽���
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("Player gained %d HP!"), HealAmount));

        if (ASBPawnCharacter* PlayerCharacter = Cast<ASBPawnCharacter>(Activator))
        {
            // ĳ������ ü���� ȸ��
            PlayerCharacter->AddHealth(HealAmount);
            PlayerCharacter->UpdateOverheadHP();
        }

        DestroyItem();
    }
}
                                                                                                                                                                                                                                                                         