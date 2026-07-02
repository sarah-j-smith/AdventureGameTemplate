// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"

#include "Gameplay/VerbInteractions.h"
#include "DescribableItem.h"
#include "StoryAction.h"
#include "ItemDataList.h"

#include "InventoryItem.generated.h"

class UBarkProvider;
class UManagerProvider;
class UItem;

/**
 * The `Item` in our inventory, that can be scripted with blueprints.
 */
UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class ADVENTURETOOLS_API UInventoryItem : public UObject, 
    public IVerbInteractions, public IDescribableItem
{
    GENERATED_BODY()
    
    UStoryAction* ItemDataAssetForAction(EVerbType Verb) const;
    
    UPROPERTY()
    UManagerProvider* ManagerProvider;
    
    UPROPERTY()
    UBarkProvider* BarkProvider;
    
    bool bHandled = false;

public:
    UInventoryItem();
    
    virtual FText GetShortDescription() const override;

    virtual FText GetLongDescription() const override;

    virtual FName GetItemKind() const override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemHandling")
    UItem *ItemDetails;

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
