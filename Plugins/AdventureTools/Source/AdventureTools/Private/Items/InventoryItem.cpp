// (c) 2025 Sarah Smith


#include "Items/InventoryItem.h"
#include "Items/AssetActionComponent.h"

#include "Constants.h"
#include "AdventureTools.h"
#include "Item.h"
#include "Provider.h"
#include "VerbType.h"
#include "Gameplay/BarkProvider.h"
#include "Player/AdventurePlayerController.h"
#include "Player/ItemManager.h"

#include "Internationalization/StringTableRegistry.h"

    


TSharedPtr<IManagerProvider> UInventoryItem::GetManagerProvider()
{
    if (_ManagerProvider.IsValid()) return _ManagerProvider;
    _ManagerProvider = UProvider::Get()->GetInstance<IManagerProvider>();
    return _ManagerProvider;
}

TSharedPtr<IBarkProvider> UInventoryItem::GetBarkProvider()
{
    if (_BarkProvider.IsValid()) return _BarkProvider;
    _BarkProvider = UProvider::Get()->GetInstance<IBarkProvider>();
    return _BarkProvider;
}

FGameplayTagContainer& UInventoryItem::GetTagContainer()
{
    return HistoryTags;
}

void UInventoryItem::PlayerBarkAndEnd(FText Text)
{
    _BarkProvider->BarkAndEnd(Text, this);
}

void UInventoryItem::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
    TagContainer.AppendTags(HistoryTags);
    TagContainer.AppendTags(StateTags);
    TagContainer.AppendTags(ItemTags);
}

//////////////////////////////////
///
/// STATIC IMPLEMENTATIONS
///

void UInventoryItem::OnItemActionSuccess_Implementation()
{
    UE_LOG(LogAdventureGame, Log, TEXT("OnItemUseSuccess Success - default."));
    bHandled = false;
    const ACommandManager *Command = GetManagerProvider()->GetCommandManager(this);
    if (UStoryAction *ItemDataAsset = ItemDataAssetForAction(Command->CurrentVerb))
    {
        ItemDataAsset->bHandled = false;
        Command->AssetActionComponent->OnItemActionSuccess(ItemDataAsset);
        if (ItemDataAsset->bHandled)
        {
            bHandled = true;
            ItemDataAsset->bHandled = false;
        }
    }
}

void UInventoryItem::OnItemActionFailure_Implementation()
{
    UE_LOG(LogAdventureGame, Log, TEXT("OnItemUseSuccess Success - default."));
    bHandled = false;
    const ACommandManager *Command = GetManagerProvider()->GetCommandManager(this);
    if (UStoryAction *ItemDataAsset = ItemDataAssetForAction(Command->CurrentVerb))
    {
        ItemDataAsset->bHandled = false;
        Command->AssetActionComponent->OnItemActionFailure(ItemDataAsset);
        if (ItemDataAsset->bHandled)
        {
            bHandled = true;
            ItemDataAsset->bHandled = false;
        }
    }
}

UStoryAction* UInventoryItem::ItemDataAssetForAction(const EVerbType Verb) const
{
    return ItemDetails->Activations.GetItemDataAssetForAction(Verb);
}

void UInventoryItem::OnClose_Implementation()
{
    IVerbInteractions::OnClose_Implementation();
    if (CanCloseDoorOrItem(ItemDetails->DoorState))
    {
        OnItemActionSuccess();
    }
    else
    {
        OnItemActionFailure();
    }
    if (bHandled) return;
    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("On close"));
    GetBarkProvider()->BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "CloseDefaultText"), this);
}

void UInventoryItem::OnOpen_Implementation()
{
    IVerbInteractions::OnOpen_Implementation();
    if (CanOpenDoorOrItem(ItemDetails->DoorState))
    {
        OnItemActionSuccess();
    }
    else
    {
        OnItemActionFailure();
    }
    if (bHandled) return;
    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("On open"));
    GetBarkProvider()->BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "OpenDefaultText"), this);
}

void UInventoryItem::OnGive_Implementation()
{
    IVerbInteractions::OnGive_Implementation();
    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("On give inventory item defaultasdfasdfasdf"));
    _BarkProvider->BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "GiveDefaultText"), this);
}

void UInventoryItem::OnPickUp_Implementation()
{
    IVerbInteractions::OnPickUp_Implementation();
    OnItemActionSuccess();
    if (bHandled) return;
    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("On Pickup"));
    GetBarkProvider()->BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "PickUpDefaultText"), this);
}

void UInventoryItem::OnTalkTo_Implementation()
{
    IVerbInteractions::OnTalkTo_Implementation();
    OnItemActionSuccess();
    if (bHandled) return;
    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("On talk"));
    GetBarkProvider()->BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "TalkToDefaultText"), this);
}

void UInventoryItem::OnLookAt_Implementation()
{
    IVerbInteractions::OnLookAt_Implementation();
    OnItemActionSuccess();
    if (bHandled) return;
    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("On look at"));
    if (ItemDetails->Description.IsEmpty())
    {
        GetBarkProvider()->BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "LookAtDefaultText"), this);
    }
    else
    {
        GetBarkProvider()->BarkAndEnd(ItemDetails->Description, this);
    }
}

void UInventoryItem::OnPull_Implementation()
{
    IVerbInteractions::OnPull_Implementation();
    OnItemActionSuccess();
    if (bHandled) return;
    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("On pull"));
    GetBarkProvider()->BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "PullDefaultText"), this);
}

void UInventoryItem::OnPush_Implementation()
{
    IVerbInteractions::OnPush_Implementation();
    OnItemActionSuccess();
    if (bHandled) return;
    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("On push"));
    GetBarkProvider()->BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "PushDefaultText"), this);
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
    GetBarkProvider()->BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "WalkToDefaultText"), this);
}

void UInventoryItem::OnItemUsed_Implementation()
{
    IVerbInteractions::OnItemUsed_Implementation();

    // **this** InventoryItem is the target and ItemManager->SourceItem is the source of a Use
    // verb. Check that the Source can validly use on this.
    if (UItemManager *ItemManager = GetManagerProvider()->GetItemManager(this))
    {
        if (ItemManager->CanInteractWith(ItemDetails->ItemTypeDef))
        {
            // Item is used on itself - failure - this should not be necessary,
            // but needed in the case that during game design this item mistakenly
            // has its interactable item set to another with the same item kind.
            if (ACommandManager *Command = GetManagerProvider()->GetCommandManager(this))
            {
                Command->InterruptCurrentAction();
            }
            OnItemActionFailure();
        }
        else if (const UItem *Src = ItemManager->GetSourceItem(); Src->InteractableItemName == ItemDetails->ItemTypeDef)
        {
            // This item has interactable item
            OnItemActionSuccess();
        }
        else if (const UStoryAction *ItemDataAsset = ItemDataAssetForAction(EVerbType::UseItem))
        {
            // We are the target, the second item clicked
            if (ItemDataAsset->SourceItem == ItemManager->SourceItemTag && ItemDataAsset->TargetItem == ItemManager->TargetItemTag)
            {
                OnItemActionSuccess();
            }
            else if (ItemDataAsset->SourceItem == ItemManager->TargetItemTag && 
                ItemDataAsset->TargetItem == ItemManager->SourceItemTag && ItemDataAsset->CanSwapSourceAndTarget)
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
    // Usually we give an item to an NPC (a hot-spot) and giving it to another item is weird.
    GetBarkProvider()->BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "ItemGivenDefaultText"), this);
}
