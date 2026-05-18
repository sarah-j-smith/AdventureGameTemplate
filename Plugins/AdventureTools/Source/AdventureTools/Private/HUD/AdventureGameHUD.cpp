// (c) 2025 Sarah Smith


#include "HUD/AdventureGameHUD.h"

#include "AdventureTools.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Image.h"

#include "GameUtils.h"
#include "Enums/SaveGameStatus.h"
#include "BarkRequest.h"
#include "BarkText.h"

#include "Internationalization/StringTableRegistry.h"
#include "Player/ItemManager.h"
#include "Player/CommandManager.h"
#include "Player/InteractionNotifier.h"

#include "Gameplay/AdventureGameInstance.h"
#include "Gameplay/AdventureGameMode.h"
#include "Items/InventoryItem.h"

#include "HUD/InteractionHUD.h"
#include "HUD/PromptList.h"
#include "HUD/InventoryUI.h"
#include "HUD/VerbsUI.h"

#include "HotSpots/HotSpot.h"

#include "DescribableItem.h"
#include "Constants.h"

void UAdventureGameHUD::NativeOnInitialized()
{
    if (UGameplayStatics::GetPlatformName() == "IOS" || UGameplayStatics::GetPlatformName() == "Android")
    {
        IsMobileTouch = true;
    }

    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("UAdventureGameHUD::NativeOnInitialized"));
}

UAdventureGameHUD* UAdventureGameHUD::Create(APlayerController *PlayerController, TSubclassOf<UAdventureGameHUD> AdventureHUDClass)
{
    check(AdventureHUDClass);
    UAdventureGameHUD *AdventureHUDWidget = CreateWidget<UAdventureGameHUD>(PlayerController, AdventureHUDClass, TEXT("Adventure HUD"));
    if (AdventureHUDWidget)
    {
        AdventureHUDWidget->AddToViewport();
        UE_LOG(LogAdventureGame, VeryVerbose, TEXT("AAdventureGameModeBase::SetupHUD - AddToViewport"));
    }
    else
    {
        UE_LOG(LogAdventureGame, Warning, TEXT("Could not create HUD!"));
    }
    return AdventureHUDWidget;
}

void UAdventureGameHUD::BindCommandHandlers(ACommandManager *CommandManager)
{
    check(CommandManager);

    CommandManager->BeginAction.AddUObject(this, &UAdventureGameHUD::BeginActionEvent);
    CommandManager->UpdateInteractionTextDelegate.AddUObject(this, &UAdventureGameHUD::UpdateInteractionTextEvent);
    CommandManager->InterruptAction.AddUniqueDynamic(this, &UAdventureGameHUD::InterruptActionEvent);
    
    if (UItemManager *ItemManager = CommandManager->ItemManager)
    {
        ItemManager->UpdateInventoryTextDelegate.AddUObject(this, &UAdventureGameHUD::UpdateInventoryTextEvent);
    }
}

void UAdventureGameHUD::BindInventoryHandlers(UAdventureGameInstance* AdventureGameInstance)
{
    AdventureGameInstance->PlayerInventoryChanged.AddUniqueDynamic(this, &UAdventureGameHUD::HandleInventoryChanged);
}

void UAdventureGameHUD::BindScoreHandlers(AAdventureGameModeBase *AdventureGameMode)
{
    AdventureGameMode->ScoreDelegate.AddUniqueDynamic(this, &UAdventureGameHUD::HandleScoreChanged);
}

void UAdventureGameHUD::BindNotifierHandlers(UInteractionNotifier* Notifier)
{
    Notifier->UserInteraction.AddUObject(this, &UAdventureGameHUD::OnUserInteracted);
    Notifier->PromptListOpenRequest.AddUObject(this, &UAdventureGameHUD::ShowPromptList);
    Notifier->PromptListCloseRequest.AddUObject(this, &UAdventureGameHUD::HidePromptList);
}

void UAdventureGameHUD::ShowBlackScreen()
{
    if (IsValid(BlackScreen))
    {
        BlackScreen->SetVisibility(ESlateVisibility::Visible);
    }
}

void UAdventureGameHUD::HideBlackScreen()
{
    if (IsValid(BlackScreen))
    {
        BlackScreen->SetVisibility(ESlateVisibility::Hidden);
    }
}

void UAdventureGameHUD::SetInteractionText()
{
    const ACommandManager *Command = GetCommandManager();
    const UItemManager *ItemManager = GetItemManager();
    if (!Command || !ItemManager) return;
    auto Verb = Command->CurrentVerb;
    const UInventoryItem* SourceItem = ItemManager->SourceItem;
    if ((Verb == EVerbType::GiveItem || Verb == EVerbType::UseItem) && SourceItem == nullptr)
    {
        UE_LOG(LogAdventureGame, Warning, TEXT("Tried to %s with no source item"),
               *VerbGetDescriptiveString(Verb).ToString());
        return;
    }
    if (AHotSpot *CurrentHotspot = Command->CurrentHotSpot)
    {
        FText InteractionText;
        switch (Verb)
        {
        case EVerbType::UseItem:
            InteractionText = FGameUtils::GetUsingItemText(Cast<IDescribableItem>(SourceItem),
                nullptr, Cast<IDescribableItem>(CurrentHotspot));
            break;
        case EVerbType::Give:
            // Can't just give a hotspot - have to give _something_ to the hotspot - eg GiveItem
            InteractionText = LOCTABLE(ITEM_STRINGS_KEY, "GiveDefaultText");
            break;
        case EVerbType::GiveItem:
            InteractionText = FGameUtils::GetGivingItemText(Cast<IDescribableItem>(SourceItem), nullptr,
                Cast<IDescribableItem>(CurrentHotspot));
            break;
        default:
            InteractionText = FGameUtils::GetVerbWithHotSpotText(Cast<IDescribableItem>(CurrentHotspot), Verb);
        }
        InteractionUI->SetText(InteractionText);
        UE_LOG(LogAdventureGame, Log, TEXT("Set interaction text to: %s"), *InteractionText.ToString());
        if (Command->ShouldHighlightInteractionText())
        {
            InteractionUI->HighlightText();
        }
    }
    else
    {
        FText VerbStr = VerbGetDescriptiveString(Verb);
        InteractionUI->SetText(VerbStr);
    }
}

void UAdventureGameHUD::SetInventoryText()
{
    const ACommandManager *Command = GetCommandManager();
    const UItemManager *ItemManager = GetItemManager();
    if (!Command || !ItemManager) return;
    const EVerbType Verb = Command->CurrentVerb;
    FText InventoryText;
    const UInventoryItem* SourceItem = ItemManager->SourceItem;
    const UInventoryItem* TargetItem = ItemManager->TargetItem;
    const AHotSpot* HotSpot = Command->CurrentHotSpot;
    if (SourceItem == nullptr)
    {
        InteractionUI->ResetText();
        return;
    }
    const IDescribableItem *SourceItemDescribable = Cast<IDescribableItem>(SourceItem);
    const IDescribableItem *TargetItemDescribable = Cast<IDescribableItem>(TargetItem);
    const IDescribableItem *HotSpotDescribable = Cast<IDescribableItem>(HotSpot);
#if WITH_EDITOR
    const FString SourceDebugStr = SourceItemDescribable ? SourceItemDescribable->GetShortDescription().ToString() : TEXT("NO SRC");
    const FString TargetDebugStr = TargetItemDescribable ? TargetItemDescribable->GetShortDescription().ToString() : TEXT("NO TGT");
    const FString HotSpotDebugStr = HotSpotDescribable ? HotSpotDescribable->GetShortDescription().ToString() : TEXT("NO HOTSPOT");
    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("Set inventory text: %s - source item: %s - target item: %s - hotspot: %s"),
        *VerbGetDescriptiveString(Verb).ToString(), *SourceDebugStr, *TargetDebugStr, *HotSpotDebugStr);
#endif
    switch (Verb)
    {
    case EVerbType::Use:
    case EVerbType::UseItem:
        InventoryText = FGameUtils::GetUsingItemText(SourceItemDescribable, TargetItemDescribable, HotSpotDescribable);
        break;
    case EVerbType::GiveItem:
    case EVerbType::Give:
        InventoryText = FGameUtils::GetGivingItemText(SourceItemDescribable, TargetItemDescribable, HotSpotDescribable);
        break;
    default:
        InventoryText = FGameUtils::GetVerbWithItemText(SourceItemDescribable, Verb);
    }
    InteractionUI->SetText(InventoryText);
    if (Command->ShouldHighlightInteractionText())
    {
        InteractionUI->HighlightText();
    }
}

void UAdventureGameHUD::ShowPromptList()
{
    if (UISwitcher->GetActiveWidget() != PromptList)
    {
        DefaultWidget = UISwitcher->GetActiveWidget();
        UISwitcher->SetActiveWidget(PromptList);
    }
}

void UAdventureGameHUD::HidePromptList()
{
    if (UISwitcher->GetActiveWidget() != DefaultWidget)
    {
        UISwitcher->SetActiveWidget(DefaultWidget);
    }
}

void UAdventureGameHUD::AddBarkText(const TArray<FText>& BarkTextArray, USphereComponent* Position,
                                    TOptional<FColor> TextColor)
{
    Bark->AddBarkRequest(FBarkRequest::CreateNPCMultilineRequest(BarkTextArray, 0, Position,
                                                                 TextColor.Get(
                                                                     G_Player_Default_Text_Colour.ToFColor(true))));
}

void UAdventureGameHUD::AddBarkText(const FText& BarkText, USphereComponent* Position, TOptional<FColor> TextColor)
{
    FColor Col = TextColor.Get(G_Player_Default_Text_Colour.ToFColor(true));
    Bark->AddBarkRequest(FBarkRequest::CreateNPCRequest(BarkText, 0, Position, Col));
}

void UAdventureGameHUD::ClearBarkText()
{
    Bark->ClearText();
}

void UAdventureGameHUD::HandleInventoryChanged(EItemKind /*ItemKind*/, EItemDisposition /*Disposition*/)
{
    // TO-DO: Possibly handle changes instead of destroying and re-importing.
    InventoryUI->PopulateInventory(true);
}

void UAdventureGameHUD::HandleScoreChanged(int32 Score)
{
    InteractionUI->SetScore(FGameUtils::GetScoreText(Score));
}

void UAdventureGameHUD::UpdateInteractionTextEvent()
{
    SetInteractionText();
}

void UAdventureGameHUD::UpdateSaveGameIndicatorEvent(const ESaveGameStatus SaveGameStatus, bool Success)
{
    switch (SaveGameStatus)
    {
    case ESaveGameStatus::Saved:
        InteractionUI->EndSaving(Success);
        break;
    case ESaveGameStatus::Saving:
        InteractionUI->StartSaving();
        break;
    case ESaveGameStatus::Loading:
        InteractionUI->StartLoading();
        break;
    case ESaveGameStatus::Loaded:
        InteractionUI->EndLoading(Success);
        break;
    default:
        // Do nothing
        break;
    }
}

void UAdventureGameHUD::UpdateInventoryTextEvent()
{
    SetInventoryText();
}

void UAdventureGameHUD::BeginActionEvent()
{
    InteractionUI->HighlightText();
}

void UAdventureGameHUD::InterruptActionEvent()
{
    InteractionUI->ResetText();
    VerbsUI->ClearActiveButton();
}

void UAdventureGameHUD::OnUserInteracted()
{
    VerbsUI->ClearActiveButton();
    Bark->OnUserInteracted();
}

void UAdventureGameHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (!IsMobileTouch)
    {
        if (ACommandManager *Command = GetCommandManager())
        {
            bool MouseIsOverUI = IsHovered();
            Command->UpdateMouseOverUI(MouseIsOverUI);
        }
    }
}
