// (c) 2025 Sarah Smith


#include "Items/ItemDataAsset.h"

#include "Items/InventoryItem.h"
#include "Constants.h"
#include "Player/AdventurePlayerController.h"
#include "AdventureGameplayTags.h"
#include "AdventureTools.h"
#include "HotSpots/Door.h"
#include "Player/ItemManager.h"

void UItemDataAsset::OnItemGiveSuccess_Implementation()
{
    if (UItemManager *ItemManager = GetItemManager())
    {
        ItemManager->AddToScore(ScoreOnSuccess);
        ItemManager->ItemRemoveFromInventoryAsync(SourceItem);
    }
    StartTimer();
}

void UItemDataAsset::OnItemUseSuccess_Implementation()
{
    UItemManager *ItemManager = GetItemManager();
    const ACommandManager *CommandManager = GetCommandManager();
    if (!ItemManager || !CommandManager) return;
    ItemManager->AddToScore(ScoreOnSuccess);
    bool Success = true;
    TSet<EItemAssetType> ItemAssetTypes = AdventureGameplayTags::GetItemAssetTypes(SourceItemTreatmentTags);
    ItemAssetTypes.Add(SourceItemAssetType);
    for (const EItemAssetType& ItemAssetType : ItemAssetTypes)
    {
        HandleSourceItem(ItemAssetType, Success);
    }

    TSet<EItemAssetType> TargetItemAssetTypes = AdventureGameplayTags::GetItemAssetTypes(TargetItemTreatmentTags);
    TargetItemAssetTypes.Add(TargetItemAssetType);
    for (const EItemAssetType& ItemAssetType : TargetItemAssetTypes)
    {
        HandleTargetItem(ItemAssetType, Success);
    }
    BarkAndEnd(Success ? UseSuccessBarkText : UseFailureBarkText);
}

void UItemDataAsset::HandleSourceItem(const EItemAssetType ItemAssetType, bool &Success)
{
    UItemManager *ItemManager = GetItemManager();
    switch (ItemAssetType)
    {
    case EItemAssetType::Consumable:
        ItemManager->ItemRemoveFromInventoryAsync(SourceItem);
        break;
    case EItemAssetType::Tool:
        ItemManager->ItemAddToInventory(ToolResultItem);
        break;
    case EItemAssetType::Key:
        HandleKeyCase(Success);
        break;
    default:
        UE_LOG(LogAdventureGame, Error, TEXT("Item data asset has bad asset type: %s"),
            *UEnum::GetValueAsString(SourceItemAssetType));
        break;
    }
}

void UItemDataAsset::HandleTargetItem(const EItemAssetType ItemAssetType, bool& Success)
{
    UItemManager *ItemManager = GetItemManager();
    switch (ItemAssetType)
    {
    case EItemAssetType::Consumable:
        ItemManager->ItemRemoveFromInventoryAsync(TargetItem);
        break;
    default:
        break;
    }
}

void UItemDataAsset::HandleKeyCase(bool &Success)
{
    const ACommandManager *CommandManager = GetCommandManager();
    UItemManager *ItemManager = GetItemManager();
    if (AHotSpot* ThisHotSpot = CommandManager->CurrentHotSpot)
    {
        if (ADoor* Door = Cast<ADoor>(ThisHotSpot))
        {
            Success = Door->UnlockDoor();
            if (!Success && Door->DoorState != EDoorState::Locked)
            {
                Bark(LOCTABLE(ITEM_STRINGS_KEY, "AlreadyUnlocked"));
            }
        }
    }
    else if (CanUnlockDoorOrItem(ItemManager->TargetItem->DoorState))
    {
        Success = true;
        ItemManager->TargetItem->DoorState = EDoorState::Closed;
    }
}

void UItemDataAsset::OnItemGiveFailure_Implementation()
{
    BarkAndEnd(GiveFailureBarkText);
}

void UItemDataAsset::OnItemUseFailure_Implementation()
{
    BarkAndEnd(UseFailureBarkText);
}

void UItemDataAsset::OnInteractionTimeout()
{
    if (ACommandManager *CommandManager = GetCommandManager())
    {
        CommandManager->InterruptCurrentAction();
    }
}

void UItemDataAsset::StartTimer()
{
    if (TimerRunning) return;
    TimerRunning = true;
    GetWorld()->GetTimerManager().SetTimer(
        ActionHighlightTimerHandle, this,
        &UItemDataAsset::OnInteractionTimeout,
        InteractionTimeout, false);
}

void UItemDataAsset::StopTimer()
{
    if (!TimerRunning) return;
    {
        TimerRunning = false;
        GetWorld()->GetTimerManager().ClearTimer(ActionHighlightTimerHandle);
    }
}
