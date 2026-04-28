// (c) 2025 Sarah Smith


#include "Gameplay/AdventureGameModeBase.h"

#include "AdventureTools.h"

AAdventureGameModeBase::AAdventureGameModeBase()
{
	UE_LOG(LogAdventureGame, VeryVerbose, TEXT("Construct: AAdventureGameModeBase - Custom Game Mode class"));
}

void AAdventureGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogAdventureGame, VeryVerbose, TEXT("AAdventureGameModeBase::BeginPlay"));
}

void AAdventureGameModeBase::AddToScore(int32 Increment)
{
	Score += Increment;
	ScoreDelegate.Broadcast(Score);
}
