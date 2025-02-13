// Fill out your copyright notice in the Description page of Project Settings.


#include "CoinItem.h"
#include "SBGameState.h"

ACoinItem::ACoinItem()
{

}

void ACoinItem::ActivateItem(AActor* Activator)
{
    if (Activator && Activator->ActorHasTag("Player"))
    {
		if (UWorld* World = GetWorld())
		{
			if (ASBGameState* GameState = World->GetGameState<ASBGameState>())
			{
				GameState->AddScore(PointValue);
				GameState->OnCoinCollected();
			}
		}

        DestroyItem();
    }
}
