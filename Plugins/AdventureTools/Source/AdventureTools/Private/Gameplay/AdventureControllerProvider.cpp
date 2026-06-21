// (c) 2026 Storybridge Games


#include "Gameplay/AdventureControllerProvider.h"

#include "AdventureTools.h"
#include "Player/AdventurePlayerController.h"
#include "Player/AdventureCharacter.h"
#include "Kismet/GameplayStatics.h"

AAdventurePlayerController* UAdventureControllerProvider::GetAdventurePlayerController(UObject* WorldContextObject)
{
	static TWeakObjectPtr<AAdventurePlayerController> CachedAdventureController;
	if (AAdventurePlayerController* AdventureController = CachedAdventureController.Get())
	{
		return AdventureController;
	}
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0);
	AAdventurePlayerController* AdventurePlayerController = Cast<AAdventurePlayerController>(PlayerController);
	if (AdventurePlayerController && IsValid(AdventurePlayerController))
	{
		CachedAdventureController = AdventurePlayerController;
		return AdventurePlayerController;
	}
	// Could happen if the level is being torn down or a loading of a save game is in progress
	UE_LOG(LogAdventureGame, Display, TEXT("Adventure player controller not available in %s"),
	       *WorldContextObject->GetName());
	return nullptr;
}

AAdventureCharacter* UAdventureControllerProvider::GetAdventureCharacter(UObject* WorldContextObject)
{
	// If there is an AdventureController then it should have a current handle to the AdventureCharacter
	// and if it doesn't then its probably not ready or available, so return it without checking.
	if (const AAdventurePlayerController* AdventureController = GetAdventurePlayerController(WorldContextObject))
	{
		return AdventureController->PlayerCharacter;
	}
	// If there is NOT an AdventureController then its likely because we are in testing mode, in which
	// case there is no AAdventurePlayerController at all. Search for AAdventureCharacter in the scene.
	AActor* Actor = UGameplayStatics::GetActorOfClass(WorldContextObject, AAdventureCharacter::StaticClass());
	if (AAdventureCharacter* AdventureCharacter = Cast<AAdventureCharacter>(Actor))
	{
		return AdventureCharacter;
	}
	// Could happen if the level is being torn down or a loading of a save game is in progress
	UE_LOG(LogAdventureGame, Display, TEXT("Adventure player character not available in %s "),
	       *WorldContextObject->GetName());
	return nullptr;
}
