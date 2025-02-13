#include "SBGameInstance.h"

USBGameInstance::USBGameInstance()
{
	TotalScore = 0;
	CurrentLevelIndex = 0;
}

void USBGameInstance::AddToScore(int32 Amount)
{
	TotalScore += Amount;
	UE_LOG(LogTemp, Warning, TEXT("Total Score Updated: %d"), TotalScore);
}