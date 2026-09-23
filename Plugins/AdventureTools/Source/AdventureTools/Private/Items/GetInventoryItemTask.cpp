// (c) 2025 Sarah Smith

#include "Items/GetInventoryItemTask.h"
#include "Items/InventoryItem.h"
#include "ItemDisposition.h"
#include "AdventureTools.h"
#include "Provider.h"
#include "Gameplay/AdventureGameInstance.h"
#include "Kismet/GameplayStatics.h"

UGetInventoryItemTask* UGetInventoryItemTask::DoGetInventoryItemTask(
    const UObject* WorldContextObject, const FName ItemKind, const float WaitTime)
{
    UGetInventoryItemTask* Task = NewObject<UGetInventoryItemTask>(const_cast<UObject*>(WorldContextObject));
    Task->WorldContextObject = WorldContextObject;
    Task->ItemKind = ItemKind;
    Task->WaitTime = WaitTime;
    Task->InventoryManager = UProvider::Get()->GetInstance<IInventoryManager>();

    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("GetInventoryItemTask created for %s"), *ItemKind.ToString());
    Task->RegisterWithGameInstance(WorldContextObject);
    return Task;
}

void UGetInventoryItemTask::Activate()
{
    Super::Activate();

    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("GetInventoryItemTask::Activate - %s"), *ItemKind.ToString());

    if (UItem *Item = InventoryManager->GetItemFromInventory(ItemKind))
    {
        if (CheckForSuccessCondition()) return;
    }
    InventoryManager->NotifyInventoryChanged([this](FInventoryChangedDelegate &Delegate)
    {
        Delegate.AddUObject(this, &UGetInventoryItemTask::OnPlayerInventoryChanged);
    });
    StartWaitTimer();
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
    // Check one last time in case its there, but if not fail
    if (CheckForSuccessCondition()) return;
    TaskFailed.Broadcast();
    SetReadyToDestroy();
}

void UGetInventoryItemTask::OnPlayerInventoryChanged(FName ChangedItemKind, EItemDisposition Disposition)
{
    if (CheckForSuccessCondition()) return;
    
    // Not what we are looking for, keep waiting but log it in case somehow misconfigured
    UE_LOG(LogAdventureGame, Display, TEXT("Waiting for %s - but saw - %s - %s"),
        *ItemKind.ToString(), *UEnum::GetValueAsString(Disposition),
        *ChangedItemKind.ToString());
}

bool UGetInventoryItemTask::CheckForSuccessCondition()
{
    if (UItem *Item = InventoryManager->GetItemFromInventory(ItemKind))
    {
        TaskSuccessful.Broadcast(Item);
        SetReadyToDestroy();
        return true;
    }
    return false;
}
