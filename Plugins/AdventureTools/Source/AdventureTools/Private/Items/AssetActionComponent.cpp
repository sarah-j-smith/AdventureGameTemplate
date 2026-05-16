// (c) 2025 Sarah Smith


#include "Items/AssetActionComponent.h"

#include "AdventureGameplayTags.h"
#include "AdventureTools.h"
#include "Constants.h"
#include "Player/ItemManager.h"
#include "Player/CommandManager.h"
#include "HotSpots/Door.h"
#include "Items/InventoryItem.h"
#include "Internationalization/StringTableRegistry.h"


// Sets default values for this component's properties
UAssetActionComponent::UAssetActionComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;

    // ...
}


// Called when the game starts
void UAssetActionComponent::BeginPlay()
{
    Super::BeginPlay();

    // ...
    
}


// Called every frame
void UAssetActionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // ...
}


void UAssetActionComponent::OnItemGiveSuccess_Implementation(UItemDataAsset *DataAsset)
{
    if (UItemManager *ItemManager = GetItemManager())
    {
        ItemManager->AddToScore(DataAsset->ScoreOnSuccess);
        ItemManager->ItemRemoveFromInventoryAsync(DataAsset->SourceItem);
    }
    StartTimer(DataAsset);
}

void UAssetActionComponent::OnItemUseSuccess_Implementation(UItemDataAsset *DataAsset)
{
    UItemManager *ItemManager = GetItemManager();
    const ACommandManager *CommandManager = GetCommandManager();
    if (!ItemManager || !CommandManager) return;
    ItemManager->AddToScore(DataAsset->ScoreOnSuccess);
    bool Success = true;
    TSet<EItemAssetType> ItemAssetTypes = AdventureGameplayTags::GetItemAssetTypes(DataAsset->SourceItemTreatmentTags);
    ItemAssetTypes.Add(DataAsset->SourceItemAssetType);
    for (const EItemAssetType& ItemAssetType : ItemAssetTypes)
    {
        HandleSourceItem(DataAsset, ItemAssetType, Success);
    }

    TSet<EItemAssetType> TargetItemAssetTypes = AdventureGameplayTags::GetItemAssetTypes(DataAsset->TargetItemTreatmentTags);
    TargetItemAssetTypes.Add(DataAsset->TargetItemAssetType);
    for (const EItemAssetType& ItemAssetType : TargetItemAssetTypes)
    {
        HandleTargetItem(DataAsset, ItemAssetType, Success);
    }
    BarkAndEnd(Success ? DataAsset->UseSuccessBarkText : DataAsset->UseFailureBarkText);
}


void UAssetActionComponent::HandleSourceItem(UItemDataAsset *DataAsset, const EItemAssetType ItemAssetType, bool &Success)
{
    UItemManager *ItemManager = GetItemManager();
    switch (ItemAssetType)
    {
    case EItemAssetType::Consumable:
        ItemManager->ItemRemoveFromInventoryAsync(DataAsset->SourceItem);
        break;
    case EItemAssetType::Tool:
        ItemManager->ItemAddToInventory(DataAsset->ToolResultItem);
        break;
    case EItemAssetType::Key:
        HandleKeyCase(Success);
        break;
    default:
        UE_LOG(LogAdventureGame, Error, TEXT("Item data asset has bad asset type: %s"),
            *UEnum::GetValueAsString(DataAsset->SourceItemAssetType));
        break;
    }
}

void UAssetActionComponent::HandleTargetItem(UItemDataAsset *DataAsset, const EItemAssetType ItemAssetType, bool& Success)
{
    UItemManager *ItemManager = GetItemManager();
    switch (ItemAssetType)
    {
    case EItemAssetType::Consumable:
        ItemManager->ItemRemoveFromInventoryAsync(DataAsset->TargetItem);
        break;
    default:
        break;
    }
}

void UAssetActionComponent::HandleKeyCase(bool &Success)
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

void UAssetActionComponent::OnItemGiveFailure_Implementation(UItemDataAsset *DataAsset)
{
    BarkAndEnd(DataAsset->GiveFailureBarkText);
}

void UAssetActionComponent::OnItemUseFailure_Implementation(UItemDataAsset *DataAsset)
{
    BarkAndEnd(DataAsset->UseFailureBarkText);
}

void UAssetActionComponent::OnInteractionTimeout()
{
    if (ACommandManager *CommandManager = GetCommandManager())
    {
        CommandManager->InterruptCurrentAction();
    }
}

void UAssetActionComponent::StartTimer(UItemDataAsset *DataAsset)
{
    if (TimerRunning) return;
    TimerRunning = true;
    GetWorld()->GetTimerManager().SetTimer(
        ActionHighlightTimerHandle, this,
        &UAssetActionComponent::OnInteractionTimeout,
        DataAsset->InteractionTimeout, false);
}

void UAssetActionComponent::StopTimer()
{
    if (!TimerRunning) return;
    {
        TimerRunning = false;
        GetWorld()->GetTimerManager().ClearTimer(ActionHighlightTimerHandle);
    }
}
