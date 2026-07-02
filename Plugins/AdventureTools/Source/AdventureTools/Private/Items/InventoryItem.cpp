// (c) 2025 Sarah Smith


#include "Items/InventoryItem.h"
#include "Items/AssetActionComponent.h"

#include "Constants.h"
#include "AdventureTools.h"
#include "Item.h"
#include "VerbType.h"
#include "Gameplay/ManagerProvider.h"
#include "Gameplay/BarkProvider.h"
#include "Player/AdventurePlayerController.h"
#include "Player/ItemManager.h"

#include "Internationalization/StringTableRegistry.h"


UInventoryItem::UInventoryItem()
{
    ManagerProvider = CreateDefaultSubobject<UManagerProvider>("ManagerProvider");
    BarkProvider = CreateDefaultSubobject<UBarkProvider>("BarkProvider");
}

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
    bHandled = false;
    const ACommandManager *Command = ManagerProvider->GetCommandManager(this);
    if (UStoryAction *ItemDataAsset = ItemDataAssetForAction(Command->CurrentVerb))
    {
        ItemDataAsset->bHandled = false;
        Command->AssetActionComponent->OnItemActionSuccess(ItemDataAsset);
        if (ItemDataAsset->bHandled)
        {
            bHandled = true;
            ItemDataAsset->bHandled = false;
            return;
        }
    }
    OnItemActionSuccess();
}

void UInventoryItem::OnItemActionFailure_Implementation()
{
    UE_LOG(LogAdventureGame, Log, TEXT("OnItemUseSuccess Success - default."));
    bHandled = false;
    const ACommandManager *Command = ManagerProvider->GetCommandManager(this);
    if (UStoryAction *ItemDataAsset = ItemDataAssetForAction(Command->CurrentVerb))
    {
        ItemDataAsset->bHandled = false;
        Command->AssetActionComponent->OnItemActionFailure(ItemDataAsset);
        if (ItemDataAsset->bHandled)
        {
            bHandled = true;
            ItemDataAsset->bHandled = false;
            return;
        }
    }
    OnItemActionFailure();
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
    BarkProvider->BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "CloseDefaultText"), this);
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
    BarkProvider->BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "OpenDefaultText"), this);
}

void UInventoryItem::OnGive_Implementation()
{
    IVerbInteractions::OnGive_Implementation();
    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("On give inventory item defaultasdfasdfasdf"));
    BarkProvider->BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "GiveDefaultText"), this);
}

void UInventoryItem::OnPickUp_Implementation()
{
    IVerbInteractions::OnPickUp_Implementation();
    OnItemActionSuccess();
    if (bHandled) return;
    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("On Pickup"));
    BarkProvider->BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "PickUpDefaultText"), this);
}

void UInventoryItem::OnTalkTo_Implementation()
{
    IVerbInteractions::OnTalkTo_Implementation();
    OnItemActionSuccess();
    if (bHandled) return;
    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("On talk"));
    BarkProvider->BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "TalkToDefaultText"), this);
}

void UInventoryItem::OnLookAt_Implementation()
{
    IVerbInteractions::OnLookAt_Implementation();
    OnItemActionSuccess();
    if (bHandled) return;
    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("On look at"));
    if (ItemDetails->Description.IsEmpty())
    {
        BarkProvider->BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "LookAtDefaultText"), this);
    }
    else
    {
        BarkProvider->BarkAndEnd(ItemDetails->Description, this);
    }
}

void UInventoryItem::OnPull_Implementation()
{
    IVerbInteractions::OnPull_Implementation();
    OnItemActionSuccess();
    if (bHandled) return;
    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("On pull"));
    BarkProvider->BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "PullDefaultText"), this);
}

void UInventoryItem::OnPush_Implementation()
{
    IVerbInteractions::OnPush_Implementation();
    OnItemActionSuccess();
    if (bHandled) return;
    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("On push"));
    BarkProvider->BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "PushDefaultText"), this);
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
    BarkProvider->BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "WalkToDefaultText"), this);
}

void UInventoryItem::OnItemUsed_Implementation()
{
    IVerbInteractions::OnItemUsed_Implementation();

    // **this** InventoryItem is the target and APC->SourceItem is the source of a Use
    // verb. Check that the Source can validly use on this.
    if (UItemManager *ItemManager = ManagerProvider->GetItemManager(this))
    {
        if (ItemManager->SourceItemName == ItemDetails->ItemTypeDef)
        {
            // Item is used on itself - failure - this should not be necessary,
            // but needed in the case that during game design this item mistakenly
            // has its interactable item set to another with the same item kind.
            if (ACommandManager *Command = ManagerProvider->GetCommandManager(this))
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
    // Usually we give an item to an NPC (a hot-spot) and giving it to another item is weird.
    BarkProvider->BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "ItemGivenDefaultText"), this);
}
