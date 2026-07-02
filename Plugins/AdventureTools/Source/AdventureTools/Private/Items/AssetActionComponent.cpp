// (c) 2025 Sarah Smith


#include "Items/AssetActionComponent.h"

#include "AdventureGameplayTags.h"
#include "AdventureTools.h"
#include "Constants.h"
#include "Gameplay/ManagerProvider.h"
#include "Player/ItemManager.h"
#include "Player/CommandManager.h"
#include "HotSpots/Door.h"
#include "Gameplay/ManagerProvider.h"
#include "Internationalization/StringTableRegistry.h"
#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
UAssetActionComponent::UAssetActionComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;

    ManagerProvider = CreateDefaultSubobject<UManagerProvider>("ManagerProvider");
}


// Called when the game starts
void UAssetActionComponent::BeginPlay()
{
    Super::BeginPlay();
    
}


// Called every frame
void UAssetActionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // ...
}

void UAssetActionComponent::OnItemActionSuccess_Implementation(UStoryAction *DataAsset)
{
    UItemManager *ItemManager = ManagerProvider->GetItemManager(this);
    const ACommandManager *CommandManager = ManagerProvider->GetCommandManager(this);
    ensureAlways(ItemManager);
    ensureAlways(CommandManager);
    ItemManager->AddToScore(DataAsset->ScoreOnSuccess);
    bool Success = true;
    TSet<EItemAssetType> ItemAssetTypes = AdventureGameplayTags::GetItemAssetTypes(DataAsset->SourceItemTreatmentTags);
    for (const EItemAssetType& ItemAssetType : ItemAssetTypes)
    {
        HandleSourceItem(DataAsset, ItemAssetType, Success);
    }
    if (Success)
    {
        TSet<EItemAssetType> TargetItemAssetTypes = AdventureGameplayTags::GetItemAssetTypes(DataAsset->TargetItemTreatmentTags);
        for (const EItemAssetType& ItemAssetType : TargetItemAssetTypes)
        {
            HandleTargetItem(DataAsset, ItemAssetType, Success);
        }
    }
    UGameplayStatics::PlaySound2D(GetWorld(), Success ? DataAsset->ActionSuccessSound : DataAsset->ActionFailureSound);
    UPlayerBarkManager *PlayerBarkManager = CommandManager->GetBarkController();
    PlayerBarkManager->PlayerBarkAndEnd(Success ? DataAsset->ActionSuccessBarkText : DataAsset->ActionFailureBarkText);
    DataAsset->bHandled = true;
}

void UAssetActionComponent::HandleSourceItem(UStoryAction *DataAsset, const EItemAssetType ItemAssetType, bool &Success)
{
    UItemManager *ItemManager = ManagerProvider->GetItemManager(this);
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
            *UEnum::GetValueAsString(ItemAssetType));
        break;
    }
}

void UAssetActionComponent::HandleTargetItem(UStoryAction *DataAsset, const EItemAssetType ItemAssetType, bool& Success)
{
    UItemManager *ItemManager = ManagerProvider->GetItemManager(this);
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
    const ACommandManager *CommandManager = ManagerProvider->GetCommandManager(this);
    const UItemManager *ItemManager = ManagerProvider->GetItemManager(this);
    if (AHotSpot* ThisHotSpot = CommandManager->CurrentHotSpot)
    {
        if (ADoor* Door = Cast<ADoor>(ThisHotSpot))
        {
            Success = Door->UnlockDoor();
            if (!Success && Door->DoorState != EDoorState::Locked)
            {
                UPlayerBarkManager *BarkManager = CommandManager->GetBarkController();
                BarkManager->PlayerBark(LOCTABLE(ITEM_STRINGS_KEY, "AlreadyUnlocked"));
            }
        }
        return;
    }
    if (const UItem *Target = ItemManager->GetTargetItem())
    {
        if (CanUnlockDoorOrItem(Target->DoorState))
        {
            Success = true; 
            ItemManager->GetTargetItem()->DoorState = EDoorState::Closed; // Closed but not locked
        }
    }
}

void UAssetActionComponent::OnItemActionFailure_Implementation(UStoryAction *DataAsset)
{
    const ACommandManager *CommandManager = ManagerProvider->GetCommandManager(this);
    UPlayerBarkManager *BarkManager = CommandManager->GetBarkController();
    BarkManager->PlayerBarkAndEnd(DataAsset->ActionFailureBarkText);
    UGameplayStatics::PlaySound2D(GetWorld(), DataAsset->ActionFailureSound);
    DataAsset->bHandled = true;
}

void UAssetActionComponent::OnInteractionTimeout()
{
    if (ACommandManager *CommandManager = ManagerProvider->GetCommandManager(this))
    {
        CommandManager->InterruptCurrentAction();
    }
}

void UAssetActionComponent::StartTimer(UStoryAction *DataAsset)
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
