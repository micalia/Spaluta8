// Fill out your copyright notice in the Description page of Project Settings.


#include "SBGameStateBase.h"

ASBGameStateBase::ASBGameStateBase()
{
    Score = 0;
}

int32 ASBGameStateBase::GetScore() const
{
    return Score;
}

void ASBGameStateBase::AddScore(int32 Amount)
{
    Score += Amount;
}