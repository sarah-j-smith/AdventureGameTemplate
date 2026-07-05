// (c) 2025 Sarah Smith


#include "Player/ItemManager.h"

#include "AdventureTools.h"
#include "Item.h"
#include "ItemDisposition.h"
#include "ItemTypeDefs.h"
#include "Gameplay/AdventureGameInstance.h"
#include "Gameplay/AdventureGameModeBase.h"
#include "HUD/ItemSlot.h"
#include "Items/InventoryItem.h"
#include "Inventory.h"

#include "Kismet/GameplayStatics.h"

UItemManager::UItemManager()
{
    PrimaryComponentTick.bCanEverTick = true;
    
}

void UItemManager::OnComponentCreated()
{
    Super::OnComponentCreated();
    
    UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
    UAdventureGameInstance *AdventureGameInstance = Cast<UAdventureGameInstance>(GameInstance);        
    AdventureGameInstance->PlayerInventoryChanged.AddDynamic(this, &UItemManager::OnInventoryChanged);
    AdventureGameInstance->CustomInventoryItemLoadedDelegate.BindUObject(this, &UItemManager::CreateCustomInventoryItemHandler);
    Inventory = AdventureGameInstance->Inventory;
}

void UItemManager::OnInventoryChanged(FName ItemKind, EItemDisposition ItemDisposition)
{
    if (ItemDisposition == EItemDisposition::Reloaded)
    {
        UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
        const UAdventureGameInstance *AdventureGameInstance = Cast<UAdventureGameInstance>(GameInstance);        
        Inventory = AdventureGameInstance->Inventory;
    }
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
    UAdventureGameInstance *AdventureGameInstance = Cast<UAdventureGameInstance>(UGameplayStatics::GetGameInstance(this));
    AdventureGameInstance->GetCustomInventoryItem(ItemDef);
}

void UItemManager::CreateCustomInventoryItemHandler(FName ItemDef, UInventoryItem *InventoryItem)
{
    ensureAlwaysMsgf(!ItemDef.IsNone(), TEXT("CreateCustomInventoryItemHandler: empty Item name"));
    if (InventoryItem == nullptr)
    {
        CreateDefaultInventoryItem(ItemDef);
        return;
    }
    if (Source->ItemTypeDef.GetTagLeafName() == ItemDef)
    {
        if (SourceItem) SourceItem->ItemDetails = nullptr;
        SourceItem = InventoryItem;
        InventoryItem->ItemDetails = Source;
        UE_LOG(LogAdventureGame, Log, TEXT("Loaded custom source inventory class for %s"), *ItemDef.ToString());
    }
    else if (Target->ItemTypeDef.GetTagLeafName() == ItemDef)
    {
        if (TargetItem) TargetItem->ItemDetails = nullptr;
        TargetItem = InventoryItem;
        InventoryItem->ItemDetails = Target;
        UE_LOG(LogAdventureGame, Log, TEXT("Loaded custom target inventory class for %s"), *ItemDef.ToString());
    }
}

void UItemManager::CreateDefaultInventoryItem(FName ItemDef)
{
    ensureAlwaysMsgf(!ItemDef.IsNone(), TEXT("CreateDefaultInventoryItem: empty Item name"));
    const FName SourceName = Source ? Source->ItemTypeDef.GetTagLeafName() : NAME_None;
    const FName TargetName = Target ? Target->ItemTypeDef.GetTagLeafName() : NAME_None;
    ensureAlwaysMsgf(SourceName == ItemDef || TargetName == ItemDef, TEXT("Item name must be either source or target"));
    UAdventureGameInstance *AdventureGameInstance = Cast<UAdventureGameInstance>(UGameplayStatics::GetGameInstance(this));
    const FName IIName = MakeUniqueObjectName(AdventureGameInstance, UInventoryItem::StaticClass(), ItemDef, EUniqueObjectNameOptions::GloballyUnique);
    if (SourceName == ItemDef)
    {
        if (SourceItem) SourceItem->ItemDetails = nullptr;
        SourceItem = NewObject<UInventoryItem>(this, UInventoryItem::StaticClass(), IIName);
        SourceItem->ItemDetails = Source;
        UE_LOG(LogAdventureGame, Log, TEXT("Loaded custom source inventory class for %s"), *ItemDef.ToString());
        if (!ItemActionQueue.IsEmpty())
        {
            PerformItemAction(ItemActionQueue.Pop());
        }
    }
    else if (TargetName == ItemDef)
    {
        if (TargetItem) TargetItem->ItemDetails = nullptr;
        TargetItem = NewObject<UInventoryItem>(this, UInventoryItem::StaticClass(), IIName);;
        TargetItem->ItemDetails = Target;
        UE_LOG(LogAdventureGame, Log, TEXT("Loaded custom target inventory class for %s"), *ItemDef.ToString());
        if (!ItemActionQueue.IsEmpty())
        {
            PerformItemInteraction(ItemActionQueue.Pop());
        }
    }
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
        if (Source && !GameInstance->IsInInventory(Source->ItemTypeDef.GetTagLeafName())) ClearSourceItem();
        if (Target && !GameInstance->IsInInventory(Target->ItemTypeDef.GetTagLeafName())) ClearTargetItem();
    }
}

void UItemManager::ItemsRemoveFromInventory(const TSet<FName>& SetOfItemsToRemove)
{
    if (UAdventureGameInstance *GameInstance = Cast<UAdventureGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
    {
        GameInstance->RemoveItemsFromInventory(SetOfItemsToRemove);
        if (!GameInstance->IsInInventory(SourceItemName)) ClearSourceItem();
        if (!GameInstance->IsInInventory(TargetItemName)) ClearTargetItem();
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
    if (SourceLocked == EChoiceState::Unlocked)
    {
        Source = nullptr;
    }
    else
    {
        Target = nullptr;
    }
    CurrentItemSlot = nullptr;
    UpdateInventoryTextDelegate.Broadcast();
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
               *VerbGetDescriptiveString(CurrentVerb).ToString(), *TargetItem->GetShortDescription().ToString());
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
