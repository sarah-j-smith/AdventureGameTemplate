// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"
#include "DescribableItem.h"
#include "ItemDataAsset.h"
#include "ItemDataList.h"
#include "ItemManagerProvider.h"
#include "ItemKind.h"
#include "DoorState.h"
#include "Gameplay/VerbInteractions.h"
#include "Internationalization/StringTableRegistry.h"
#include "Paper2D/Classes/PaperSprite.h"
#include "Player/BarkProvider.h"
#include "InventoryItem.generated.h"

/**
 * The `Item` in our inventory.
 */
UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class ADVENTURETOOLS_API UInventoryItem : public UObject, 
    public IVerbInteractions, public IItemManagerProvider,
    public IBarkProvider, public IDescribableItem
{
    GENERATED_BODY()
public:
    virtual FText GetShortDescription() const override;

    virtual FText GetLongDescription() const override;

    virtual EItemKind GetItemKind() const override;
    
    //////////////////////////////////
    ///
    /// ITEM HANDLING
    ///

    /// Longer description used in "Look at" to make the item more real.
    /// eg "Blunt red knife with blood on it" - "Old tattered book covered with runes".
    /// By default uses the `DefaultItemDescriptionText` value in the `ItemStrings` string table.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemHandling")
    FText Description = LOCTABLE("ItemStrings", "DefaultItemDescriptionText");

    /// Various tags for the state and past actions done on this hotspot.
    /// For example can set "History.Triggered.LookAt" to enforce that a score increment
    /// is only given once, the first time the item is looked at.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scripting", meta = (Categories = "History"))
    FGameplayTagContainer HistoryTags;

    /// Very short description to distinguish this thing from others of the same kind
    /// eg "Blunt red knife" - "Old tattered book". By default uses the
    /// `DefaultItemDescriptionText` value in the `ItemStrings` string table.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemHandling")
    FText ShortDescription = LOCTABLE("ItemStrings", "DefaultItemDescriptionText");

    /// Enum ItemKind for this thing. Defaults to `None`. Add new
    /// item kinds in the `EItemKind` enum.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemHandling")
    EItemKind ItemKind = EItemKind::None;

    /// Thumbnail image to represent this item inside the inventory.
    /// Images used for the item while in the level or scene should
    /// be attached to a hotspot pickup instead.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemHandling")
    TObjectPtr<UPaperSprite> Thumbnail;

    /// Data Asset for determining results of using this item. Use Success and
    /// asset interrogation will only occur once a test against the <code>InteractableItem</code>
    /// succeeds.
    ///
    /// The Data Asset is an <b>instance</b> of the <code>ItemDataAsset</code> sub-class, not the
    /// class itself. Right-click in the content drawer, and choose <i>Miscellaneous > Data Asset</i>
    /// then choose one of the <code>ItemDataAsset</code> sub-classes to create an instance.
    /// @deprecated Use OnItemActivated instead
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemHandling")
    TSoftObjectPtr<UItemDataAsset> OnUseSuccessItem;

    /// Data Asset for determining results of successfully giving this item. This should be
    /// an <b>instance</b> of the <code>ItemDataAsset</code> sub-class, not the
    /// class itself. Right-click in the content drawer, and choose <i>Miscellaneous > Data Asset</i>
    /// then choose one of the <code>ItemDataAsset</code> sub-classes to create an instance.
    /// @deprecated Use OnItemActivated instead
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemHandling")
    TSoftObjectPtr<UItemDataAsset> OnGiveSuccessItem;

    /// Data Asset for determining results of successfully activating this item. This should be
    /// an <b>instance</b> of the <code>ItemDataAsset</code> sub-class, not the
    /// class itself. Right-click in the content drawer, and choose <i>Miscellaneous > Data Asset</i>
    /// then choose one of the <code>ItemDataAsset</code> sub-classes to create an instance.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemHandling")
    FItemDataList OnItemActivated;

private:
    UItemDataAsset* ItemDataAssetForAction(EVerbType Verb) const;

public:
    /// An item kind that can meaningfully interact with this one. Used when
    /// items are combined in the inventory.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemHandling")
    EItemKind InteractableItem = EItemKind::None;

    //////////////////////////////////
    ///
    /// EVENT HANDLERS
    ///

    /// Triggered when this item is the <b>target</b> of a use verb, and it successfully
    /// passes initial checks. The source item of the use verb is found by querying
    /// the adventure player controller's <code>SourceItem</code> property.
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ItemHandling")
    void OnItemUseSuccess();

    /// Triggered when this item is the <b>target</b> of a use verb, and it fails to
    /// pass initial checks. The source item of the use verb is found by querying
    /// the adventure player controller's <code>SourceItem</code> property.
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ItemHandling")
    void OnItemUseFailure();

    /// Triggered when this item is the <b>target</b> of a give verb, and it successfully
    /// passes initial checks. The source item of the give verb is found by querying
    /// the adventure player controller's <code>SourceItem</code> property.
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ItemHandling")
    void OnItemGiveSuccess();

    /// Triggered when this item is the <b>target</b> of a give verb, and it fails to
    /// pass initial checks. The source item of the use verb is found by querying
    /// the adventure player controller's <code>SourceItem</code> property.
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ItemHandling")
    void OnItemGiveFailure();

    //////////////////////////////////
    ///
    /// VERB INTERACTION
    ///

    UFUNCTION(BlueprintCallable, Category = "VerbInteractions")
    virtual void OnClose_Implementation() override;

    UFUNCTION(BlueprintCallable, Category = "VerbInteractions")
    virtual void OnOpen_Implementation() override;

    UFUNCTION(BlueprintCallable, Category = "VerbInteractions")
    virtual void OnGive_Implementation() override;

    UFUNCTION(BlueprintCallable, Category = "VerbInteractions")
    virtual void OnPickUp_Implementation() override;

    UFUNCTION(BlueprintCallable, Category = "VerbInteractions")
    virtual void OnTalkTo_Implementation() override;

    UFUNCTION(BlueprintCallable, Category = "VerbInteractions")
    virtual void OnLookAt_Implementation() override;

    UFUNCTION(BlueprintCallable, Category = "VerbInteractions")
    virtual void OnPull_Implementation() override;

    UFUNCTION(BlueprintCallable, Category = "VerbInteractions")
    virtual void OnPush_Implementation() override;

    UFUNCTION(BlueprintCallable, Category = "VerbInteractions")
    virtual void OnUse_Implementation() override;

    UFUNCTION(BlueprintCallable, Category = "VerbInteractions")
    virtual void OnWalkTo_Implementation() override;

    UFUNCTION(BlueprintCallable, Category = "VerbInteractions")
    virtual void OnItemUsed_Implementation() override;

    UFUNCTION(BlueprintCallable, Category = "VerbInteractions")
    virtual void OnItemGiven_Implementation() override;

    //////////////////////////////////
    ///
    /// ITEM DISPOSITION
    ///

    /// Is the item open or close, or locked?  For items where those concepts make
    /// no sense this should stay in the <code>Unknown</code> state.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemHandling")
    EDoorState DoorState = EDoorState::Unknown;

    /// If this Inventory Item can interact with <code>ItemToInteract</code> then
    /// return true, otherwise return false. 
    UFUNCTION(BlueprintCallable, Category = "Player Actions")
    bool CanInteractWith(const UInventoryItem* ItemToInteract) const
    {
        return ItemToInteract->InteractableItem == ItemKind || ItemToInteract->ItemKind == InteractableItem;
    }
};
