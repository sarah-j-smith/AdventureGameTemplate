// (c) 2025 Sarah Smith

#include "Player/BarkTask.h"

#include "AdventureTools.h"
#include "GameUtils.h"

#include "Gameplay/BarkProvider.h"
#include "Gameplay/ManagerProvider.h"
#include "Player/CommandManager.h"
#include "Player/PlayerBarkManager.h"

UBarkTask::UBarkTask(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , WorldContextObject(nullptr)
    , BarkText(FText::GetEmpty())
{
    MyUID = FGameUtils::GetUUID();
}

UBarkTask* UBarkTask::DoBarkTask(const UObject* WorldContextObject, const FText BarkText)
{
    UBarkTask* BlueprintNode = NewObject<UBarkTask>(const_cast<UObject*>(WorldContextObject));
    BlueprintNode->WorldContextObject = WorldContextObject;
    BlueprintNode->BarkText = BarkText;
    BlueprintNode->ManagerProvider = NewObject<UManagerProvider>(BlueprintNode);
    BlueprintNode->MyUID = FGameUtils::GetUUID();

    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("UBarkTask created"));

    // Register with the game instance to avoid being garbage collected
    BlueprintNode->RegisterWithGameInstance(WorldContextObject);
    return BlueprintNode;
}

void UBarkTask::Activate()
{
    Super::Activate();

    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("UBarkTask Activate - %s"), *(BarkText.ToString()));

    ACommandManager *CommandManager = ManagerProvider->GetCommandManager(this);
    check(CommandManager);
    UPlayerBarkManager *BarkManager = CommandManager->GetBarkController();
    check(BarkManager);
    BarkManager->PlayerBark(BarkText, MyUID);
    BarkManager->EndPlayerBark.AddUObject(this, &UBarkTask::BarkCompleted);
}

void UBarkTask::BarkCompleted(int32 UID, EBarkRequestFinishedReason Reason)
{
    if (UID == MyUID)
    {
        UE_LOG(LogAdventureGame, VeryVerbose, TEXT("Task completion message UID: %d - BarkText: %s"),
               UID, *(BarkText.ToString()));
        switch (Reason)
        {
        case EBarkRequestFinishedReason::Timeout:
            TaskCompleted.Broadcast();
            break;
        case EBarkRequestFinishedReason::Interruption:
            TaskInterrupted.Broadcast();
            break;
        case EBarkRequestFinishedReason::BarkAndEnd:
            TaskInterrupted.Broadcast();
            break;
        }
        SetReadyToDestroy();
    }
    else
    {
        UE_LOG(LogAdventureGame, VeryVerbose, TEXT("Ignoring task completion message UID: %d - BarkText: %s"),
               UID, *(BarkText.ToString()));
    }
}
