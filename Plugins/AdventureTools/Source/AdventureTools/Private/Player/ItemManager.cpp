// (c) 2025 Sarah Smith


#include "Player/ItemManager.h"

#include "AdventureTools.h"
#include "Item.h"
#include "Gameplay/AdventureGameInstance.h"
#include "Gameplay/AdventureGameModeBase.h"
#include "HUD/ItemSlot.h"
#include "Items/InventoryItem.h"
#include "Provider.h"
#include "Items/IInventoryManager.h"

#include "Kismet/GameplayStatics.h"

UItemManager::UItemManager()
    : InventoryManager(UProvider::Get()->GetInstance<IInventoryManager>())
{
    PrimaryComponentTick.bCanEverTick = true;
    UE_LOG(LogAdventureGame, VeryVerbose, TEXT(">>> Constructor: Item Manager %p"), this);
    InventoryManager->CustomInventoryItemLoaded([this](FCustomInventoryItemLoaded &Delegate)
    {
        Delegate.AddUObject(this, &UItemManager::CreateCustomInventoryItemHandler);
    });
}

void UItemManager::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogAdventureGame, VeryVerbose, TEXT(">>> OnComponentCreated Item Manager: %p"), this);
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
    return Source;
}

UItem* UItemManager::GetTargetItem() const
{
    return Target;
}

void UItemManager::CheckForCustomInventoryItem(FName ItemDef)
{
    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("CheckForCustomInventoryItem: %s"), *ItemDef.ToString());
    InventoryManager->GetCustomInventoryItem(ItemDef);
}

// Handle async calls returning after searching for a custom UInventoryItem for a currently 
// locked in source or target.
void UItemManager::CreateCustomInventoryItemHandler(FName ItemDef, UInventoryItem *InventoryItem)
{
    ensureAlwaysMsgf(!ItemDef.IsNone(), TEXT("CreateDefaultInventoryItem: empty Item name!"));
    ensureAlwaysMsgf(SourceLocked == EChoiceState::Locked, TEXT("Expected source to be locked!"));
    
    // Find whether its the source or target that we are waiting for. When source is locked we can load
    // a target but beyond that there can never be more queued up: we lock the UI and there should be no
    // additional fetches of inventory objects from the file system.
    const FName SourceName = GetSourceItemName();
    const FName TargetName = GetTargetItemName();
    ensureAlwaysMsgf(SourceName == ItemDef || TargetName == ItemDef, TEXT("Item name must be either source or target"));

    if (InventoryItem == nullptr)
    {
        // No custom one, create a default one.
        InventoryItem = CreateDefaultInventoryItem(ItemDef);
    }
    if (SourceName == ItemDef)
    {
        UpdateSourceWithNewInventoryItem(InventoryItem);
    }
    else if (TargetName == ItemDef)
    {
        UpdateTargetWithNewInventoryItem(InventoryItem);
    }
    if (!ItemActionQueue.IsEmpty())
    {
        ensureAlwaysMsgf(!!SourceItem || !!TargetItem, TEXT("Invariant! At least source item must be set!!"));
        PerformItemAction(ItemActionQueue.Pop());
    }
}

UInventoryItem *UItemManager::CreateDefaultInventoryItem(FName ItemDef)
{
    UAdventureGameInstance *AdventureGameInstance = Cast<UAdventureGameInstance>(UGameplayStatics::GetGameInstance(this));
    const FName IIName = MakeUniqueObjectName(AdventureGameInstance, UInventoryItem::StaticClass(), ItemDef, EUniqueObjectNameOptions::GloballyUnique);
    UInventoryItem *NewDefaultItem = NewObject<UInventoryItem>(this, UInventoryItem::StaticClass(), IIName);
    UE_LOG(LogAdventureGame, Log, TEXT("Used default %s inventory class for %s"), 
        (GetSourceItemName() == ItemDef ? TEXT("source") : TEXT("target")), *ItemDef.ToString());
    return NewDefaultItem;
}

void UItemManager::UpdateTargetWithNewInventoryItem(UInventoryItem* InventoryItem)
{
    if (TargetItem) TargetItem->ItemDetails = nullptr; // avoid possible retain loop
    TargetItem = InventoryItem;
    TargetItem->ItemDetails = Target;
}

void UItemManager::UpdateSourceWithNewInventoryItem(UInventoryItem* InventoryItem)
{
    if (SourceItem) SourceItem->ItemDetails = nullptr; // avoid possible retain loop
    SourceItem = InventoryItem;
    SourceItem->ItemDetails = Source;
}

bool UItemManager::HasSourceItem() const
{
    return !!Source || SourceLocked == EChoiceState::Locked;
}

bool UItemManager::HasTargetItem() const
{
    return !!Target || TargetLocked == EChoiceState::Locked;
}

bool UItemManager::CanInteractWith(const FGameplayTag OtherItem) const
{
    if (Source && SourceLocked == EChoiceState::Locked && Source->ItemTypeDef != OtherItem) return true;
    return false;
}

void UItemManager::SetAndLockSourceItem(UItemSlot* SourceItemSlot)
{
    ensureAlwaysMsgf(SourceItemSlot != nullptr, TEXT("Expected source item not to be null"));
    SourceLocked = EChoiceState::Locked;
    Source = SourceItemSlot->InventoryItem;
    SourceItem = nullptr;
    CheckForCustomInventoryItem(SourceItemSlot->InventoryItem->ItemTypeDef.GetTagLeafName());
}

void UItemManager::SetAndLockTargetItem(UItemSlot* TargetItemSlot)
{
    ensureAlwaysMsgf(TargetItemSlot != nullptr, TEXT("Expected target item not to be null"));
    TargetLocked = EChoiceState::Locked;
    Target = TargetItemSlot->InventoryItem;
    TargetItem = nullptr;
    CheckForCustomInventoryItem(TargetItemSlot->InventoryItem->ItemTypeDef.GetTagLeafName());
}

void UItemManager::SwapSourceAndTarget()
{
    UItem* ATargetItem = Target;
    Target = Source;
    Source = ATargetItem;
    UInventoryItem* ATargetItemToSwap = TargetItem;
    TargetItem = SourceItem;
    SourceItem = ATargetItemToSwap;
    if (TargetItem) TargetItem->ItemDetails = Target;
    if (SourceItem) SourceItem->ItemDetails = Source;
}

void UItemManager::ItemAddToInventory(const FName& ItemToAdd)
{
    InventoryManager->AddItemToInventory(ItemToAdd);
}

void UItemManager::ItemRemoveFromInventory(const FName& ItemToRemove)
{
    InventoryManager->RemoveItemFromInventory(ItemToRemove);
    if (Source && !InventoryManager->IsInInventory(GetSourceItemName())) ClearSourceItem();
    if (Target && !InventoryManager->IsInInventory(GetTargetItemName())) ClearTargetItem();
}

void UItemManager::ItemsRemoveFromInventory(const TSet<FName>& SetOfItemsToRemove)
{
    InventoryManager->RemoveItemsFromInventory(SetOfItemsToRemove);
    if (Source && !InventoryManager->IsInInventory(GetSourceItemName())) ClearSourceItem();
    if (Target && !InventoryManager->IsInInventory(GetTargetItemName())) ClearTargetItem();
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
    bool Handled = false;
    check(ItemSlot->HasItem); // should never happen as this is checked by caller
    if (TargetLocked == EChoiceState::Locked && SourceLocked == EChoiceState::Locked)
    {
#if WITH_EDITOR
        const FString DebugString = ItemSlot->InventoryItem->Description.ToString();
        UE_LOG(LogAdventureGame, Warning, TEXT("Ignoring further click on %s - source and target are locked"),
               *DebugString);
#endif
        Handled = true;
    }
    CurrentItemSlot = ItemSlot;
    return Handled;
}

void UItemManager::MouseEnterInventoryItem(UItemSlot* ItemSlot)
{
    if (SourceLocked == EChoiceState::Locked && TargetLocked == EChoiceState::Locked) return;
    if (ItemSlot->HasItem)
    {
        if (SourceLocked == EChoiceState::Unlocked)
        {
            Source = ItemSlot->InventoryItem;
        }
        else
        {
            Target = ItemSlot->InventoryItem;
        }
        CurrentItemSlot = ItemSlot;
        UpdateInventoryText();
    }
}

void UItemManager::MouseLeaveInventoryItem()
{
    if (SourceLocked == EChoiceState::Locked && TargetLocked == EChoiceState::Locked) return;
    bool InventoryDisplayChanged = false;
    if (SourceLocked == EChoiceState::Unlocked)
    {
        if (Source != nullptr) InventoryDisplayChanged = true;
        Source = nullptr;
    }
    else
    {
        if (Target != nullptr) InventoryDisplayChanged = true;
        Target = nullptr;
    }
    if (CurrentItemSlot != nullptr) InventoryDisplayChanged = true;
    CurrentItemSlot = nullptr;
    if (InventoryDisplayChanged) UpdateInventoryText();
}

void UItemManager::PerformItemInteraction(EVerbType CurrentVerb)
{
    ensureAlwaysMsgf(SourceItem, TEXT("Source item must be locked"));
    
    if (TargetItem == nullptr)
    {
        ItemActionQueue.Add(CurrentVerb);
        UE_LOG(LogAdventureGame, Log, TEXT("PerformItemInteraction - queued %s pending item load"), *UEnum::GetValueAsString(CurrentVerb));
        return;
    }
    
    switch (CurrentVerb)
    {
    case EVerbType::GiveItem:
        UInventoryItem::Execute_OnItemGiven(TargetItem);
        break;
    case EVerbType::UseItem:
        if (SourceItemTag == TargetItemTag)
        {
            // Cannot use an item on itself - fail
            TargetItem->OnItemActionFailure();
        }
        else if (CanInteractWith(TargetItemTag))
        {
            TargetItem->OnItemActionSuccess();
        }
        else if (UStoryAction *ItemDataAsset = TargetItem->ItemDetails->Activations.GetItemDataAssetForAction(EVerbType::Use))
        {
            if (ItemDataAsset->SourceItem == SourceItem->ItemDetails->ItemTypeDef)
            {
                ActionDispatch.Execute(ItemDataAsset, SourceItem);
            }
        }
        UInventoryItem::Execute_OnItemUsed(TargetItem);
        break;
    default:
        UE_LOG(LogAdventureGame, Warning, TEXT("Unexpected interaction verb %s for perform item interaction with %s"),
               *VerbGetDescriptiveString(CurrentVerb).ToString(), *Target->GetShortDescription().ToString());
    }
    UpdateInventoryText();
}

void UItemManager::PerformItemAction(EVerbType CurrentVerb)
{
#if WITH_EDITOR
    const FString DebugString = Source->ShortDescription.ToString();
    UE_LOG(LogAdventureGame, Warning, TEXT("PerformItemAction %s - %s"),
           *VerbGetDescriptiveString(CurrentVerb).ToString(), *DebugString);
#endif
    
    if (SourceItem == nullptr)
    {
        ensureAlwaysMsgf(SourceLocked == EChoiceState::Locked || TargetLocked == EChoiceState::Locked,
            TEXT("Invariant: cannot queue action if these are not locked!"));
        ItemActionQueue.Add(CurrentVerb);
        UE_LOG(LogAdventureGame, Log, TEXT("PerformItemAction - queued %s pending item load"), *UEnum::GetValueAsString(CurrentVerb));
        return;
    }
    
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

void UItemManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (ItemsToRemove.Num() > 0)
    {
        ItemsRemoveFromInventory(ItemsToRemove);
        ItemsToRemove.Empty();
    }
}
