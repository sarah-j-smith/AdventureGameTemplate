// (c) 2025 Sarah Smith


#include "Player/BarkProvider.h"
#include "Player/PlayerBarkManager.h"
#include "Player/AdventurePlayerController.h"
#include "AdventureTools.h"

// Add default functionality here for any IBarkProvider functions that are not pure virtual.

ACommandManager *Local_GetCommandManager(IBarkProvider *ContextObject)
{
    static TWeakObjectPtr<ACommandManager> CachedCommandManager;
    if (ACommandManager *CommandManager = dynamic_cast<ACommandManager *>(ContextObject))
    {
        return CommandManager;
    }
    if (ACommandManager* CommandManager = CachedCommandManager.Get())
    {
        return CommandManager;
    }
    if (const UObject *Context = dynamic_cast<UObject *>(ContextObject))
    {
        // Don't bother caching the dynamic_cast x 2 above as they're fast compared to
        // get actor of class, and likely faster even than TWeakObjectPtr.Get
        AActor* Actor = UGameplayStatics::GetActorOfClass(Context, ACommandManager::StaticClass());
        if (ACommandManager* CommandManager = Cast<ACommandManager>(Actor))
        {
            CachedCommandManager = CommandManager;
            return CommandManager;
        }
        UE_LOG(LogAdventureGame, Warning, TEXT("Command manager not available in %s"),
            *(Context->GetName()));
        return nullptr;
    }
    // Programmer error - only add IItemManagerProvider interface to UObject subclasses
    UE_LOG(LogAdventureGame, Fatal, TEXT("ItemManagerProvider illegal on non-UObject"));
    return nullptr; // never reached
}

UPlayerBarkManager *Local_GetBarkController(IBarkProvider *ContextObject)
{
    if (ACommandManager *CommandManager = Local_GetCommandManager(ContextObject))
    {
        if (UPlayerBarkManager *PlayerBarkManager = CommandManager->GetBarkController())
        {
            return PlayerBarkManager;
        }
        UE_LOG(LogAdventureGame, Warning,
            TEXT("BarkProvider did not find PlayerBarkManager on CommandManager"));
    }
    return nullptr;
}

void IBarkProvider::Bark(FText BarkText)
{
    if (UPlayerBarkManager *BarkController = Local_GetBarkController(this))
    {
        BarkController->PlayerBark(BarkText);
    }
}

void IBarkProvider::BarkAndEnd(FText BarkText)
{
    if (UPlayerBarkManager *BarkController = Local_GetBarkController(this))
    {
        BarkController->PlayerBarkAndEnd(BarkText);
    }
}

void IBarkProvider::BarkLines(TArray<FText> BarkTextArray)
{
    if (UPlayerBarkManager *BarkController = Local_GetBarkController(this))
    {
        BarkController->PlayerBarkLines(BarkTextArray);
    }
}

void IBarkProvider::ClearBark()
{
    if (UPlayerBarkManager *BarkController = Local_GetBarkController(this))
    {
        BarkController->ClearBark();
    }
}