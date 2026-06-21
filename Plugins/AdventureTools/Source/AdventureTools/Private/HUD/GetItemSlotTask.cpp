// (c) 2025 Sarah Smith

#include "HUD/GetItemSlotTask.h"

#include "AdventureTools.h"

#include "HUD/AdventureGameHUD.h"
#include "HUD/InventoryUI.h"

UGetItemSlotTask* UGetItemSlotTask::DoGetItemSlotTask(const UObject* WorldContextObject, UAdventureGameHUD* HUD,
                                                      FName ItemKind, float WaitTime)
{
    check(HUD);
    check(HUD->InventoryUI);
    check(WorldContextObject);
    check(!ItemKind.IsNone());
    
    UGetItemSlotTask* Task = NewObject<UGetItemSlotTask>(HUD);
    Task->ItemKind = ItemKind;
    Task->WaitTime = WaitTime;
    Task->WorldContextObject = WorldContextObject;
    Task->InventoryHUD = HUD->InventoryUI;
    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("GetInventoryItemTask created for %s"), *ItemKind.ToString());
    Task->RegisterWithGameInstance(WorldContextObject);
    return Task;
}

void UGetItemSlotTask::Activate()
{
    Super::Activate();

    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("UGetItemSlotTask::Activate - %s"), *ItemKind.ToString());

    if (UInventoryUI *InventoryUI = InventoryHUD.Get())
    {
        if (CheckForSuccessCondition(InventoryUI))
        {
            return;
        }
        InventoryUI->InventoryUIChanged.AddUniqueDynamic(this, &UGetItemSlotTask::OnInventoryUIChanged);
        StartWaitTimer();
    }
}

void UGetItemSlotTask::StartWaitTimer()
{
    WorldContextObject->GetWorld()->GetTimerManager().SetTimer(
    WaitTimer, this, &UGetItemSlotTask::WaitTimerTimeout,
    WaitTime, false
    );
}

void UGetItemSlotTask::WaitTimerTimeout()
{
    if (UInventoryUI *InventoryUI = InventoryHUD.Get())
    {
        // Check one last time in case its there, but if not fail
        if (CheckForSuccessCondition(InventoryUI)) return;
    }
    else
    {
        UE_LOG(LogAdventureGame, Warning, TEXT("InventoryUI went away in %hs"), __FUNCTION__);
    }
    TaskFailed.Broadcast();
    SetReadyToDestroy();
}

void UGetItemSlotTask::OnInventoryUIChanged()
{
    if (UInventoryUI *InventoryUI = InventoryHUD.Get())
    {
        if (CheckForSuccessCondition(InventoryUI)) return;
        
        // Not what we are looking for, keep waiting but log it in case somehow misconfigured
        UE_LOG(LogAdventureGame, Display, TEXT("Waiting for %s - but saw a change"),
            *ItemKind.ToString());
    }
    else
    {
        UE_LOG(LogAdventureGame, Warning, TEXT("InventoryUI went away in %hs"), __FUNCTION__);
    }
}

bool UGetItemSlotTask::CheckForSuccessCondition(const UInventoryUI* InventoryUI)
{
    check(InventoryUI);
    if (UItemSlot *Item = InventoryUI->GetFromInventory(ItemKind))
    {
        TaskSuccessful.Broadcast(Item);
        SetReadyToDestroy();
        return true;
    }
    return false;
}
