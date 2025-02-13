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
        // 회복 디버그 메시지
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("Player gained %d HP!"), HealAmount));

        if (ASBPawnCharacter* PlayerCharacter = Cast<ASBPawnCharacter>(Activator))
        {
            // 캐릭터의 체력을 회복
            PlayerCharacter->AddHealth(HealAmount);
            PlayerCharacter->UpdateOverheadHP();
        }

        DestroyItem();
    }
}
                                                                                                                                                                                                                                                                         