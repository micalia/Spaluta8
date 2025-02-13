// Fill out your copyright notice in the Description page of Project Settings.


#include "SBGameMode.h"
#include "SBPawnCharacter.h"
#include "SBGameState.h"
#include "SpartaPlayerController.h"

ASBGameMode::ASBGameMode()
{
	DefaultPawnClass = ASBPawnCharacter::StaticClass();
	PlayerControllerClass = ASpartaPlayerController::StaticClass();
	GameStateClass = ASBGameState::StaticClass();
}
