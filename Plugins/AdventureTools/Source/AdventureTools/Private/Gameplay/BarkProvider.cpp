// (c) 2026 Storybridge Games


#include "Gameplay/BarkProvider.h"

#include "Gameplay/ManagerProvider.h"
#include "Player/CommandManager.h"
#include "Player/PlayerBarkManager.h"

UBarkProvider::UBarkProvider()
{
    ManagerProvider = CreateDefaultSubobject<UManagerProvider>("ItemManagerProvider");
}

void UBarkProvider::Bark(FText BarkText, UObject* WorldContextObject)
{
    const ACommandManager *CommandManager = ManagerProvider->GetCommandManager(WorldContextObject);
    if (CommandManager == nullptr) return;
    if (UPlayerBarkManager *BarkController = CommandManager->GetBarkController())
    {
        BarkController->PlayerBark(BarkText);
    }
}

void UBarkProvider::BarkAndEnd(FText BarkText, UObject* WorldContextObject)
{
    const ACommandManager *CommandManager = ManagerProvider->GetCommandManager(WorldContextObject);
    if (CommandManager == nullptr) return;
    if (UPlayerBarkManager *BarkController = CommandManager->GetBarkController())
    {
        BarkController->PlayerBarkAndEnd(BarkText);
    }
}

void UBarkProvider::BarkLines(TArray<FText> BarkTextArray, UObject* WorldContextObject)
{
    const ACommandManager *CommandManager = ManagerProvider->GetCommandManager(WorldContextObject);
    if (CommandManager == nullptr) return;
    if (UPlayerBarkManager *BarkController = CommandManager->GetBarkController())
    {
        BarkController->PlayerBarkLines(BarkTextArray);
    }
}

void UBarkProvider::ClearBark(UObject* WorldContextObject)
{
    const ACommandManager *CommandManager = ManagerProvider->GetCommandManager(WorldContextObject);
    if (CommandManager == nullptr) return;
    if (UPlayerBarkManager *BarkController = CommandManager->GetBarkController())
    {
        BarkController->ClearBark();
    }
}