// (c) 2026 Storybridge Games


#include "Gameplay/BarkProvider.h"

#include "Provider.h"
#include "Gameplay/ManagerProvider.h"
#include "Player/CommandManager.h"
#include "Player/PlayerBarkManager.h"

FBarkProvider::FBarkProvider()
    : ManagerProvider(UProvider::Get()->GetInstance<IManagerProvider>())
{
}

FBarkProvider::~FBarkProvider()
{
    // RAAI - nothing to do
}

void FBarkProvider::Bark(FText BarkText, UObject* WorldContextObject)
{
    const ACommandManager *CommandManager = ManagerProvider->GetCommandManager(WorldContextObject);
    if (CommandManager == nullptr) return;
    if (UPlayerBarkManager *BarkController = CommandManager->GetBarkController())
    {
        BarkController->PlayerBark(BarkText);
    }
}

void FBarkProvider::BarkAndEnd(FText BarkText, UObject* WorldContextObject)
{
    const ACommandManager *CommandManager = ManagerProvider->GetCommandManager(WorldContextObject);
    if (CommandManager == nullptr) return;
    if (UPlayerBarkManager *BarkController = CommandManager->GetBarkController())
    {
        BarkController->PlayerBarkAndEnd(BarkText);
    }
}

void FBarkProvider::BarkLines(TArray<FText> BarkTextArray, UObject* WorldContextObject)
{
    const ACommandManager *CommandManager = ManagerProvider->GetCommandManager(WorldContextObject);
    if (CommandManager == nullptr) return;
    if (UPlayerBarkManager *BarkController = CommandManager->GetBarkController())
    {
        BarkController->PlayerBarkLines(BarkTextArray);
    }
}

void FBarkProvider::ClearBark(UObject* WorldContextObject)
{
    const ACommandManager *CommandManager = ManagerProvider->GetCommandManager(WorldContextObject);
    if (CommandManager == nullptr) return;
    if (UPlayerBarkManager *BarkController = CommandManager->GetBarkController())
    {
        BarkController->ClearBark();
    }
}