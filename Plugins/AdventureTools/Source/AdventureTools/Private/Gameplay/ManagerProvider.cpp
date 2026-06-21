// (c) 2026 Storybridge Games

#include "Gameplay/ManagerProvider.h"
#include "Player/CommandManager.h"
#include "AdventureTools.h"
#include "Kismet/GameplayStatics.h"

ACommandManager* UManagerProvider::GetCommandManager(UObject *WorldContextObject)
{
	static TWeakObjectPtr<ACommandManager> CachedCommandManager;
	if (ACommandManager* CommandManager = CachedCommandManager.Get()) return CommandManager;
	AActor* Actor = UGameplayStatics::GetActorOfClass(WorldContextObject, ACommandManager::StaticClass());
	ACommandManager* CommandManager = Cast<ACommandManager>(Actor);
	if (!IsValid(CommandManager))
	{
		// Could happen if the level is being torn down or a loading of a save game is in progress
		UE_LOG(LogAdventureGame, Warning, TEXT("Command manager %p not available in %s"),
			CommandManager, *(WorldContextObject->GetName()));
		return nullptr;
	}
	CachedCommandManager = CommandManager;
	return CommandManager;
}

UItemManager* UManagerProvider::GetItemManager(UObject *WorldContextObject)
{
	if (const ACommandManager* CommandManager = GetCommandManager(WorldContextObject)) return CommandManager->ItemManager;
	return nullptr;
}

