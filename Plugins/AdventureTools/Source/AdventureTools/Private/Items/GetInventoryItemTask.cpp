// (c) 2025 Sarah Smith

#include "Items/GetInventoryItemTask.h"
#include "Items/InventoryItem.h"
#include "ItemDisposition.h"
#include "AdventureTools.h"
#include "Gameplay/AdventureGameInstance.h"
#include "Kismet/GameplayStatics.h"

UGetInventoryItemTask* UGetInventoryItemTask::DoGetInventoryItemTask(
    const UObject* WorldContextObject, const FName ItemKind, const float WaitTime)
{
    UGetInventoryItemTask* Task = NewObject<UGetInventoryItemTask>(const_cast<UObject*>(WorldContextObject));
    Task->WorldContextObject = WorldContextObject;
    Task->ItemKind = ItemKind;
    Task->WaitTime = WaitTime;

    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("GetInventoryItemTask created for %s"), *ItemKind.ToString());
    Task->RegisterWithGameInstance(WorldContextObject);
    return Task;
}

void UGetInventoryItemTask::Activate()
{
    Super::Activate();

    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("GetInventoryItemTask::Activate - %s"), *ItemKind.ToString());

    if (UAdventureGameInstance *GameInstance = GetAdventureGameInstance())
    {
        if (UItem *Item = GameInstance->GetItemFromInventory(ItemKind))
        {
            if (CheckForSuccessCondition(GameInstance)) return;
        }
        GameInstance->PlayerInventoryChanged.AddUniqueDynamic(this, &UGetInventoryItemTask::OnPlayerInventoryChanged);
        StartWaitTimer();
    }
    else
    {
        UE_LOG(LogAdventureGame, Error, TEXT("Could not find Adventure Game Instance in %hs"), __FUNCTION__);
        TaskFailed.Broadcast();
    }
}

void UGetInventoryItemTask::StartWaitTimer()
{
    WorldContextObject->GetWorld()->GetTimerManager().SetTimer(
        WaitTimer, this, &UGetInventoryItemTask::WaitTimerTimeout,
        WaitTime, false
        );
}

void UGetInventoryItemTask::WaitTimerTimeout()
{
    if (UAdventureGameInstance *GameInstance = GetAdventureGameInstance())
    {
        // Check one last time in case its there, but if not fail
        if (CheckForSuccessCondition(GameInstance)) return;
    }
    TaskFailed.Broadcast();
    SetReadyToDestroy();
}

void UGetInventoryItemTask::OnPlayerInventoryChanged(FName ChangedItemKind, EItemDisposition Disposition)
{
    if (UAdventureGameInstance *GameInstance = GetAdventureGameInstance())
    {
        if (CheckForSuccessCondition(GameInstance)) return;
        
        // Not what we are looking for, keep waiting but log it in case somehow misconfigured
        UE_LOG(LogAdventureGame, Display, TEXT("Waiting for %s - but saw - %s - %s"),
            *ItemKind.ToString(), *UEnum::GetValueAsString(Disposition),
            *ChangedItemKind.ToString());
    }
}

UAdventureGameInstance* UGetInventoryItemTask::GetAdventureGameInstance()
{
    if (UAdventureGameInstance* Instance = AdventureGameInstance.Get()) return Instance;
    if (UAdventureGameInstance* Instance = Cast<UAdventureGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject)))
    {
        AdventureGameInstance = Instance;
        return Instance;
    }
    UE_LOG(LogAdventureGame, Warning, TEXT("Could not find AdventureGameInstance in %hs"), __FUNCTION__);
    return nullptr;
}

bool UGetInventoryItemTask::CheckForSuccessCondition(UAdventureGameInstance* GameInstance)
{
    if (UItem *Item = GameInstance->GetItemFromInventory(ItemKind))
    {
        TaskSuccessful.Broadcast(Item);
        SetReadyToDestroy();
        return true;
    }
    return false;
}
