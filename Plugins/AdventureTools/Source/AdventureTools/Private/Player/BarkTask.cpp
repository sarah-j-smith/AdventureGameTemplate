// (c) 2025 Sarah Smith

#include "Player/BarkTask.h"

#include "AdventureTools.h"
#include "GameUtils.h"

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
    UBarkTask* BlueprintNode = NewObject<UBarkTask>();
    BlueprintNode->WorldContextObject = WorldContextObject;
    BlueprintNode->BarkText = BarkText;

    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("UBarkTask created"));

    // Register with the game instance to avoid being garbage collected
    BlueprintNode->RegisterWithGameInstance(WorldContextObject);
    return BlueprintNode;
}

void UBarkTask::Activate()
{
    Super::Activate();

    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("UBarkTask Activate - %s"), *(BarkText.ToString()));

    if (ACommandManager *CommandManager = GetCommandManager())
    {
        if (UPlayerBarkManager *BarkManager = CommandManager->GetBarkController())
        {
            BarkManager->PlayerBark(BarkText, MyUID);
            BarkManager->EndPlayerBark.AddUObject(this, &UBarkTask::BarkCompleted);
        }
    }
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
        }
        SetReadyToDestroy();
    }
    else
    {
        UE_LOG(LogAdventureGame, VeryVerbose, TEXT("Ignoring task completion message UID: %d - BarkText: %s"),
               UID, *(BarkText.ToString()));
    }
}
