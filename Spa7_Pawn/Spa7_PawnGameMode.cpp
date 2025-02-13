// Copyright Epic Games, Inc. All Rights Reserved.

#include "Spa7_PawnGameMode.h"
#include "Spa7_PawnCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "SBPawnCharacter.h"

ASpa7_PawnGameMode::ASpa7_PawnGameMode()
{
	DefaultPawnClass = ASBPawnCharacter::StaticClass();

}
