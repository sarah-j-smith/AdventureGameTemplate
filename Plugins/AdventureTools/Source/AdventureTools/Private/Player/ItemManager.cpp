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
        
    SourceItem = NewObject<UInventoryItem>(this, FName("Default SourceItem"));
    TargetItem = NewObject<UInventoryItem>(this, FName("Default TargetItem"));
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
    return SourceItem ? SourceItem->ItemDetails : nullptr;
}

UItem* UItemManager::GetTargetItem() const
{
    return TargetItem ? TargetItem->ItemDetails : nullptr;
}

void UItemManager::CheckForCustomInventoryItem(FName ItemDef)
{
    UAdventureGameInstance *AdventureGameInstance = Cast<UAdventureGameInstance>(UGameplayStatics::GetGameInstance(this));
    AdventureGameInstance->GetCustomInventoryItem(ItemDef);
}

void UItemManager::CreateCustomInventoryItemHandler(FName ItemDef, UInventoryItem *InventoryItem)
{
    if (InventoryItem == nullptr)
    {
        CreateDefaultInventoryItem(ItemDef);
        return;
    }
    if (Source->ItemTypeDef == ItemDef)
    {
        if (SourceItem) SourceItem->ItemDetails = nullptr;
        SourceItem = InventoryItem;
        InventoryItem->ItemDetails = Source;
        UE_LOG(LogAdventureGame, Log, TEXT("Loaded custom source inventory class for %s"), *ItemDef.ToString());
    }
    else if (Target->ItemTypeDef == ItemDef)
    {
        if (TargetItem) TargetItem->ItemDetails = nullptr;
        TargetItem = InventoryItem;
        InventoryItem->ItemDetails = Target;
        UE_LOG(LogAdventureGame, Log, TEXT("Loaded custom target inventory class for %s"), *ItemDef.ToString());
    }
}

void UItemManager::CreateDefaultInventoryItem(FName ItemDef)
{
    if (Source->ItemTypeDef == ItemDef)
    {
        if (SourceItem ) SourceItem->ItemDetails = nullptr;
        SourceItem = NewObject<UInventoryItem>(this);
        SourceItem->ItemDetails = Source;
        UE_LOG(LogAdventureGame, Log, TEXT("Loaded custom source inventory class for %s"), *ItemDef.ToString());
    }
    else if (Target->ItemTypeDef == ItemDef)
    {
        if (TargetItem) TargetItem->ItemDetails = nullptr;
        TargetItem = NewObject<UInventoryItem>(this);;
        TargetItem->ItemDetails = Target;
        UE_LOG(LogAdventureGame, Log, TEXT("Loaded custom target inventory class for %s"), *ItemDef.ToString());
    }
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
    ensureAlwaysMsgf(SourceItemSlot != nullptr, TEXT("Expected source item not to be null"));
    SourceLocked = EChoiceState::Locked;
    Source = SourceItemSlot->InventoryItem;
    CheckForCustomInventoryItem(SourceItemSlot->InventoryItem->ItemTypeDef);
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
        else if (const UStoryAction *ItemDataAsset = TargetItem->ItemDetails->Activations.GetItemDataAssetForAction(EVerbType::Use))
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
