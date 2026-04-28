// (c) 2025 Sarah Smith


#include "Player/ItemManager.h"

#include "AdventureTools.h"
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

bool UItemManager::CanInteractWith(EItemKind OtherItem) const
{
    if (SourceItem && SourceLocked == EChoiceState::Locked)
    {
        if (SourceItem->ItemKind != OtherItem)
        {
            return true;
        }
        else
        {
            UE_LOG(LogAdventureGame, Warning, TEXT("%s cannot target %s = same kind of item"),
                *UEnum::GetValueAsString(OtherItem), *SourceItem->GetName());
        }
    }
    else
    {
        UE_LOG(LogAdventureGame, Error, TEXT("Expected SourceItem locked in combine"));
    }
    return false;
}

void UItemManager::SwapSourceAndTarget()
{
    UInventoryItem* ATargetItem = this->TargetItem;
    this->TargetItem = this->SourceItem;
    this->SourceItem = ATargetItem;
}

UInventoryItem* UItemManager::ItemAddToInventory(const EItemKind& ItemToAdd)
{
    if (UAdventureGameInstance *GameInstance = GetAdventureGameInstance())
    {
        if (!GameInstance->IsInInventory(ItemToAdd))
        {
            if (UInventoryItem* Item = GameInstance->AddItemToInventory(ItemToAdd))
            {
                return Item;
            }
        }
#if WITH_EDITOR
        else
        {
            /// At the present even if there's two different EItemKind::Knife objects with different
            /// descriptions they are treated as the same item. To have two Knife objects with different
            /// descriptions you must create a new entry in the enum table, EItemKind::Knife2
            FString DebugString = FItemKind::GetDescription(ItemToAdd).ToString();
            UE_LOG(LogAdventureGame, Warning, TEXT("Cannot create %s - already held in inventory"),
                   *DebugString);
        }
#endif
    }
    return nullptr;
}

void UItemManager::ItemRemoveFromInventory(const EItemKind& ItemToRemove)
{
    if (UAdventureGameInstance *GameInstance = GetAdventureGameInstance())
    {
        GameInstance->RemoveItemFromInventory(ItemToRemove);
        if (SourceItem && !GameInstance->IsInInventory(SourceItem->ItemKind)) ClearSourceItem();
        if (TargetItem && !GameInstance->IsInInventory(TargetItem->ItemKind)) ClearTargetItem();
    }
}

void UItemManager::ItemsRemoveFromInventory(const TSet<EItemKind>& TItemsToRemove)
{
    if (UAdventureGameInstance *GameInstance = GetAdventureGameInstance())
    {
        GameInstance->RemoveItemsFromInventory(TItemsToRemove);
        if (SourceItem && !GameInstance->IsInInventory(SourceItem->ItemKind)) ClearSourceItem();
        if (TargetItem && !GameInstance->IsInInventory(TargetItem->ItemKind)) ClearTargetItem();
    }
}

void UItemManager::ItemRemoveFromInventoryAsync(const EItemKind& ItemToRemoveNextTick)
{
    ItemsToRemove.Add(ItemToRemoveNextTick);
}

void UItemManager::ItemsRemoveFromInventoryAsync(const TSet<EItemKind>& ItemsToRemoveNextTick)
{
    ItemsToRemove.Append(ItemsToRemoveNextTick);
}

UAdventureGameInstance* UItemManager::GetAdventureGameInstance()
{
    static TWeakObjectPtr<UAdventureGameInstance> CachedAdventureGameInstance;
    if (UAdventureGameInstance *AdventureGameInstance = CachedAdventureGameInstance.Get()) return AdventureGameInstance;

    UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
    UAdventureGameInstance* AdventureGameInstance = Cast<UAdventureGameInstance>(GameInstance);
    if (!AdventureGameInstance)
    {
        // Could happen if loading of a save game is in progress
        UE_LOG(LogAdventureGame, Warning, TEXT("Adventure Game Instance not available in %hs - %d"),
            __FUNCTION__, __LINE__);
        return nullptr;
    }
    CachedAdventureGameInstance = AdventureGameInstance;
    return AdventureGameInstance;
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
            SourceItem = ItemSlot->InventoryItem;
        }
        else
        {
            TargetItem = ItemSlot->InventoryItem;
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
        SourceItem = nullptr;
    }
    else
    {
        TargetItem = nullptr;
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
        UInventoryItem::Execute_OnItemUsed(TargetItem);
        break;
    default:
        UE_LOG(LogAdventureGame, Warning, TEXT("Unexpected interaction verb %s for perform item interaction with %s"),
               *VerbGetDescriptiveString(CurrentVerb).ToString(), *TargetItem->ShortDescription.ToString());
    }
    UpdateInventoryText();
}

void UItemManager::PerformItemAction(EVerbType CurrentVerb)
{
#if WITH_EDITOR
    const FString DebugString = SourceItem->ShortDescription.ToString();
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
