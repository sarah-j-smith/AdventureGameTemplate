// (c) 2025 Sarah Smith


#include "Items/InventoryItem.h"
#include "Items/AssetActionComponent.h"

#include "Constants.h"
#include "AdventureTools.h"
#include "Item.h"
#include "VerbType.h"
#include "Player/AdventurePlayerController.h"
#include "Player/ItemManager.h"

#include "Internationalization/StringTableRegistry.h"

FText UInventoryItem::GetShortDescription() const
{
    return ItemDetails ? ItemDetails->ShortDescription : FText::GetEmpty();
}
	
FText UInventoryItem::GetLongDescription() const
{
    return ItemDetails ? ItemDetails->Description : FText::GetEmpty();
}
	
FName UInventoryItem::GetItemKind() const
{
    return ItemDetails ? ItemDetails->ItemTypeDef : NAME_None;
}

//////////////////////////////////
///
/// STATIC IMPLEMENTATIONS
///

void UInventoryItem::OnItemActionSuccess_Implementation()
{
    UE_LOG(LogAdventureGame, Log, TEXT("OnItemUseSuccess Success - default."));
    
    if (UStoryAction *ItemDataAsset = ItemDataAssetForAction(EVerbType::UseItem))
    {
        if (const ACommandManager *Command = GetCommandManager())
        {
            Command->AssetActionComponent->OnItemActionSuccess(ItemDataAsset);
        }
        return;
    }
    OnItemActionSuccess();
}

void UInventoryItem::OnItemActionFailure_Implementation()
{
    BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "ItemUsedDefaultText"));
}

UStoryAction* UInventoryItem::ItemDataAssetForAction(const EVerbType Verb) const
{
    return OnItemActivated.GetItemDataAssetForAction(Verb);
}

void UInventoryItem::OnClose_Implementation()
{
    IVerbInteractions::OnClose_Implementation();
    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("On close"));
    BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "CloseDefaultText"));
}

void UInventoryItem::OnOpen_Implementation()
{
    IVerbInteractions::OnOpen_Implementation();
    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("On open"));
    BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "OpenDefaultText"));
}

void UInventoryItem::OnGive_Implementation()
{
    IVerbInteractions::OnGive_Implementation();
    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("On give inventory item defaultasdfasdfasdf"));
    BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "GiveDefaultText"));
}

void UInventoryItem::OnPickUp_Implementation()
{
    IVerbInteractions::OnPickUp_Implementation();
    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("On Pickup"));
    BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "PickUpDefaultText"));
}

void UInventoryItem::OnTalkTo_Implementation()
{
    IVerbInteractions::OnTalkTo_Implementation();
    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("On talk"));
    BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "TalkToDefaultText"));
}

void UInventoryItem::OnLookAt_Implementation()
{
    IVerbInteractions::OnLookAt_Implementation();
    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("On look at"));
    if (ItemDetails->Description.IsEmpty())
    {
        BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "LookAtDefaultText"));
    }
    else
    {
        BarkAndEnd(ItemDetails->Description);
    }
}

void UInventoryItem::OnPull_Implementation()
{
    IVerbInteractions::OnPull_Implementation();
    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("On pull"));
    BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "PullDefaultText"));
}

void UInventoryItem::OnPush_Implementation()
{
    IVerbInteractions::OnPush_Implementation();
    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("On push"));
    BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "PushDefaultText"));
}

void UInventoryItem::OnUse_Implementation()
{
    UE_LOG(LogAdventureGame, Fatal, TEXT("SHOULD NEVER HAPPEN"
        " - when the player clicks the Use verb and then an item "
        "the Command Manager goes into targeting mode, looking for a Hotspot"
        " or another Item to use it on."));
}

void UInventoryItem::OnWalkTo_Implementation()
{
    IVerbInteractions::OnWalkTo_Implementation();
    BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "WalkToDefaultText"));
}

void UInventoryItem::OnItemUsed_Implementation()
{
    IVerbInteractions::OnItemUsed_Implementation();

    // **this** InventoryItem is the target and APC->SourceItem is the source of a Use
    // verb. Check that the Source can validly use on this.
    if (UItemManager *ItemManager = GetItemManager())
    {
        if (ItemManager->SourceItemName == ItemDetails->ItemTypeDef)
        {
            // Item is used on itself - failure - this should not be necessary,
            // but needed in the case that during game design this item mistakenly
            // has its interactable item set to another with the same item kind.
            if (ACommandManager *Command = GetCommandManager())
            {
                Command->InterruptCurrentAction();
            }
            OnItemActionFailure();
        }
        else if (ItemManager->CanInteractWith(ItemDetails->ItemTypeDef))
        {
            // This item has interactable item
            OnItemActionSuccess();
        }
        else if (const UStoryAction *ItemDataAsset = ItemDataAssetForAction(EVerbType::UseItem))
        {
            // We are the target, the second item clicked
            if (ItemDataAsset->SourceItem == ItemManager->SourceItemName && ItemDataAsset->TargetItem == ItemManager->TargetItemName)
            {
                OnItemActionSuccess();
            }
            else if (ItemDataAsset->SourceItem == ItemManager->TargetItemName && 
                ItemDataAsset->TargetItem == ItemManager->SourceItemName && ItemDataAsset->CanSwapSourceAndTarget)
            {
                ItemManager->SwapSourceAndTarget();
                OnItemActionSuccess();
            }
        }
        else
        {
            OnItemActionFailure();
        }
    }
}

void UInventoryItem::OnItemGiven_Implementation()
{
    IVerbInteractions::OnItemGiven_Implementation();
    // **this** InventoryItem is the target and APC->SourceItem is the source of a Give verb. 
    
    // TODO Giving items to another not yet implemented - is there a use-case for this?
    BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "ItemGivenDefaultText"));
}
