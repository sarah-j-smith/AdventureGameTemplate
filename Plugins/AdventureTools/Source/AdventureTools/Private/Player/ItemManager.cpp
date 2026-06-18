// (c) 2025 Sarah Smith


#include "Player/ItemManager.h"

#include "AdventureTools.h"
#include "Item.h"
#include "Gameplay/AdventureGameInstance.h"
#include "Gameplay/AdventureGameModeBase.h"
#include "HUD/ItemSlot.h"
#include "Items/InventoryItem.h"

#include "Kismet/GameplayStatics.h"
#include "Player/AdventurePlayerController.h"

UItemManager::UItemManager()
    : SourceItem(nullptr)
    , TargetItem(nullptr)
{
    PrimaryComponentTick.bCanEverTick = true;
    
    SourceItem = CreateDefaultSubobject<UInventoryItem>(FName("SourceItem"));
    TargetItem = CreateDefaultSubobject<UInventoryItem>(FName("TargetItem"));
}

void UItemManager::AddToScore(int32 ScoreIncrement)
{
    AGameModeBase *GameMode = UGameplayStatics::GetGameMode(GetWorld());
    if (AAdventureGameModeBase *AdventureGameMode = Cast<AAdventureGameModeBase>(GameMode))
    {
        AdventureGameMode->AddToScore(ScoreIncrement);
    }
}

void UItemManager::UpdateInventoryText()
{
    if (UpdateInventoryTextDelegate.IsBound())
    {
        UpdateInventoryTextDelegate.Broadcast();
    }
}

UItem* UItemManager::GetSourceItem() const
{
    return SourceItem ? SourceItem->ItemDetails : nullptr;
}

UItem* UItemManager::GetTargetItem() const
{
    return TargetItem ? TargetItem->ItemDetails : nullptr;
}

bool UItemManager::CanInteractWith(FName OtherItem) const
{
    if (SourceItem && SourceLocked == EChoiceState::Locked)
    {
        if (SourceItem->ItemDetails->ItemTypeDef != OtherItem)
        {
            return true;
        }
        else
        {
            UE_LOG(LogAdventureGame, Warning, TEXT("%s cannot target %s = same kind of item"),
                *OtherItem.ToString(), *SourceItem->GetName());
        }
    }
    else
    {
        UE_LOG(LogAdventureGame, Error, TEXT("Expected SourceItem locked in combine"));
    }
    return false;
}

void UItemManager::SetAndLockSourceItem(UItemSlot* SourceItemSlot)
{
    SourceLocked = EChoiceState::Locked;
    SourceItem->ItemDetails = SourceItemSlot->InventoryItem;
}

void UItemManager::SetAndLockTargetItem(UItemSlot* TargetItemSlot)
{
    TargetLocked = EChoiceState::Locked;
    TargetItem->ItemDetails = TargetItemSlot->InventoryItem;
}

void UItemManager::SwapSourceAndTarget()
{
    UItem* ATargetItem = this->TargetItem->ItemDetails;
    this->TargetItem->ItemDetails = this->SourceItem->ItemDetails;
    this->SourceItem->ItemDetails = ATargetItem;
}

void UItemManager::ItemAddToInventory(const FName& ItemToAdd)
{
    if (UAdventureGameInstance *GameInstance = Cast<UAdventureGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
    {
        GameInstance->AddItemToInventory(ItemToAdd);
    }
}

void UItemManager::ItemRemoveFromInventory(const FName& ItemToRemove)
{
    if (UAdventureGameInstance *GameInstance = Cast<UAdventureGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
    {
        GameInstance->RemoveItemFromInventory(ItemToRemove);
        if (SourceItem && !GameInstance->IsInInventory(SourceItem->GetItemKind())) ClearSourceItem();
        if (TargetItem && !GameInstance->IsInInventory(TargetItem->GetItemKind())) ClearTargetItem();
    }
}

void UItemManager::ItemsRemoveFromInventory(const TSet<FName>& SetOfItemsToRemove)
{
    if (UAdventureGameInstance *GameInstance = Cast<UAdventureGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
    {
        GameInstance->RemoveItemsFromInventory(SetOfItemsToRemove);
        if (!GameInstance->IsInInventory(SourceItem->GetItemKind())) ClearSourceItem();
        if (!GameInstance->IsInInventory(TargetItem->GetItemKind())) ClearTargetItem();
    }
}

void UItemManager::ItemRemoveFromInventoryAsync(const FName& ItemToRemoveNextTick)
{
    ItemsToRemove.Add(ItemToRemoveNextTick);
}

void UItemManager::ItemsRemoveFromInventoryAsync(const TSet<FName>& ItemsToRemoveNextTick)
{
    ItemsToRemove.Append(ItemsToRemoveNextTick);
}

bool UItemManager::MaybeHandleInventoryItemClicked(UItemSlot* ItemSlot)
{
    check(ItemSlot->HasItem); // should never happen as this is checked by caller
    if (TargetLocked == EChoiceState::Locked && SourceLocked == EChoiceState::Locked)
    {
#if WITH_EDITOR
        const FString DebugString = ItemSlot->InventoryItem->Description.ToString();
        UE_LOG(LogAdventureGame, Warning, TEXT("Ignoring further click on %s - source and target are locked"),
               *DebugString);
#endif
        return false;
    }
    CurrentItemSlot = ItemSlot;
    return true;
}

void UItemManager::MouseEnterInventoryItem(UItemSlot* ItemSlot)
{
    if (SourceLocked == EChoiceState::Locked && TargetLocked == EChoiceState::Locked) return;
    if (ItemSlot->HasItem)
    {
        if (SourceLocked == EChoiceState::Unlocked)
        {
            SourceItem->ItemDetails = ItemSlot->InventoryItem;
        }
        else
        {
            TargetItem->ItemDetails = ItemSlot->InventoryItem;
        }
        CurrentItemSlot = ItemSlot;
        UpdateInventoryText();
    }
}

void UItemManager::MouseLeaveInventoryItem()
{
    if (SourceLocked == EChoiceState::Locked && TargetLocked == EChoiceState::Locked) return;
    if (SourceLocked == EChoiceState::Unlocked)
    {
        SourceItem->ItemDetails = nullptr;
    }
    else
    {
        TargetItem->ItemDetails = nullptr;
    }
    CurrentItemSlot = nullptr;
    UpdateInventoryTextDelegate.Broadcast();
}

void UItemManager::PerformItemInteraction(EVerbType CurrentVerb)
{
    check (TargetItem);
    
    switch (CurrentVerb)
    {
    case EVerbType::GiveItem:
        UInventoryItem::Execute_OnItemGiven(TargetItem);
        break;
    case EVerbType::UseItem:
        if (SourceItem->GetItemKind() == TargetItem->GetItemKind())
        {
            TargetItem->OnItemActionFailure();
        }
        else if (SourceItem->ItemDetails->CanInteractWith(TargetItem->ItemDetails))
        {
            TargetItem->OnItemActionSuccess();
        }
        else if (const UStoryAction *ItemDataAsset = TargetItem->OnItemActivated.GetItemDataAssetForAction(EVerbType::Use))
        {
            
            if (ItemDataAsset->SourceItem == SourceItem->GetItemKind())
            {
                //
            }
        }
        UInventoryItem::Execute_OnItemUsed(TargetItem);
        break;
    default:
        UE_LOG(LogAdventureGame, Warning, TEXT("Unexpected interaction verb %s for perform item interaction with %s"),
               *VerbGetDescriptiveString(CurrentVerb).ToString(), *TargetItem->GetShortDescription().ToString());
    }
    UpdateInventoryText();
}

void UItemManager::PerformItemAction(EVerbType CurrentVerb)
{
#if WITH_EDITOR
    const FString DebugString = SourceItem->GetShortDescription().ToString();
    UE_LOG(LogAdventureGame, Warning, TEXT("PerformItemAction %s - %s"),
           *VerbGetDescriptiveString(CurrentVerb).ToString(), *DebugString);
#endif

    check(SourceItem);
    switch (CurrentVerb)
    {
    case EVerbType::Give:
        UInventoryItem::Execute_OnGive(SourceItem);
        break;
    case EVerbType::Open:
        UInventoryItem::Execute_OnOpen(SourceItem);
        break;
    case EVerbType::Close:
        UInventoryItem::Execute_OnClose(SourceItem);
        break;
    case EVerbType::PickUp:
        UInventoryItem::Execute_OnPickUp(SourceItem);
        break;
    case EVerbType::LookAt:
        UInventoryItem::Execute_OnLookAt(SourceItem);
        break;
    case EVerbType::TalkTo:
        UInventoryItem::Execute_OnTalkTo(SourceItem);
        break;
    case EVerbType::Use:
        UInventoryItem::Execute_OnUse(SourceItem);
        break;
    case EVerbType::Push:
        UInventoryItem::Execute_OnPush(SourceItem);
        break;
    case EVerbType::Pull:
        UInventoryItem::Execute_OnPull(SourceItem);
        break;
    case EVerbType::WalkTo:
        UInventoryItem::Execute_OnLookAt(SourceItem);
        break;
    default:
        UE_LOG(LogAdventureGame, Warning, TEXT("Unexpected verb %s in PerformItemAction"),
               *VerbGetDescriptiveString(CurrentVerb).ToString())
    }
    UpdateInventoryText();
}

void UItemManager::BeginPlay()
{
    Super::BeginPlay();

    //
}

void UItemManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (ItemsToRemove.Num() > 0)
    {
        ItemsRemoveFromInventory(ItemsToRemove);
        ItemsToRemove.Empty();
    }
}
