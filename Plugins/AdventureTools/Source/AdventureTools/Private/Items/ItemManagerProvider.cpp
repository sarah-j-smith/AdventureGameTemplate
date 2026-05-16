// (c) 2025 Sarah Smith


#include "Items/ItemManagerProvider.h"

#include "AdventureTools.h"
#include "Player/CommandManager.h"
#include "Kismet/GameplayStatics.h"

ACommandManager* IItemManagerProvider::GetCommandManager()
{
    static TWeakObjectPtr<ACommandManager> CachedCommandManager;
    if (ACommandManager* CommandManager = CachedCommandManager.Get()) return CommandManager;
    if (UObject *WorldContextObject = dynamic_cast<UObject *>(this))
    {
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
    // Programmer error - only add IItemManagerProvider interface to UObject subclasses
    UE_LOG(LogAdventureGame, Fatal,
        TEXT("%hs - %d IItemManagerProvider only supported in UObject subclasses!"),
        __FUNCTION__, __LINE__);
    return nullptr;
}

UItemManager* IItemManagerProvider::GetItemManager()
{
    if (const ACommandManager* CommandManager = GetCommandManager()) return CommandManager->ItemManager;
    return nullptr;
}

