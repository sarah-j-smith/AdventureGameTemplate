// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"
#include "DescribableItem.h"
#include "StoryAction.h"
#include "ItemDataList.h"
#include "ItemManagerProvider.h"
#include "Gameplay/VerbInteractions.h"
#include "Player/BarkProvider.h"
#include "InventoryItem.generated.h"

class UItem;
/**
 * The `Item` in our inventory.
 */
UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class ADVENTURETOOLS_API UInventoryItem : public UObject, 
    public IVerbInteractions, public IItemManagerProvider,
    public IBarkProvider, public IDescribableItem
{
    GENERATED_BODY()
    
    UStoryAction* ItemDataAssetForAction(EVerbType Verb) const;

public:
    virtual FText GetShortDescription() const override;

    virtual FText GetLongDescription() const override;

    virtual FName GetItemKind() const override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemHandling")
    UItem *ItemDetails;
    
    /// Data Assets for determining results of activating this item. This should be
    /// a list of <b>instances</b> of the <code>ItemDataAsset</code> sub-class, not the
    /// class itself. Right-click in the content drawer, and choose <i>Miscellaneous - Data Asset</i>
    /// then choose one of the <code>ItemDataAsset</code> sub-classes to create an instance.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemHandling")
    FItemDataList OnItemActivated;

    //////////////////////////////////
    ///
    /// EVENT HANDLERS
    ///

    /// Triggered when this item is the <b>target</b> of a verb, and it successfully
    /// passes initial checks. The source item of the verb is found by querying
    /// the adventure player controller's <code>SourceItem</code> property.
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ItemHandling")
    void OnItemActionSuccess();

    /// Triggered when this item is the <b>target</b> of a verb, and it fails to
    /// pass initial checks. The source item of the use verb is found by querying
    /// the adventure player controller's <code>SourceItem</code> property.
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ItemHandling")
    void OnItemActionFailure();
    
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
};
