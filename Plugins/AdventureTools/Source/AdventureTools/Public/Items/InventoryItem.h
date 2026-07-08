// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"

#include "Gameplay/VerbInteractions.h"
#include "DescribableItem.h"
#include "GameplayTagAssetInterface.h"
#include "HistoryTagInterface.h"
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
class ADVENTURETOOLS_API UInventoryItem : public UObject, public IVerbInteractions,
    public IHistoryTagInterface, public IGameplayTagAssetInterface
{
    GENERATED_BODY()
    
    UStoryAction* ItemDataAssetForAction(EVerbType Verb) const;
    
    UPROPERTY()
    UManagerProvider* ManagerProvider;
    
    UPROPERTY()
    UBarkProvider* BarkProvider;
    
    bool bHandled = false;
    
public:
    //////////////////////////////////
    ///
    /// GAME PLAY TAG COLLECTIONS
    ///

    /// Various tags for past actions done on this hotspot.
    /// For example can set "History.Triggered.LookAt" to store that the hot spot has been looked at.
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (Category = "History"))
    FGameplayTagContainer HistoryTags;
    
    /// Various tags for the state and disposition of this item.
    /// For example can set "State.Opened" to store that the item, say a box, has been "opened".
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite,meta = (Category = "State"))
    FGameplayTagContainer StateTags;
	
    /// Various tags for the management of this item.
    /// For example can set "Item.Consumed" to store that the item, say a potion, has been "consumed" in the game.
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite ,meta = (Category = "Item"))
    FGameplayTagContainer ItemTags;
    
protected:
    /// Get the history tags for this inventory item.
    virtual FGameplayTagContainer &GetTagContainer() override;
    
public:
    UInventoryItem();
    
    //////////////////////////////////
    ///
    /// BLUEPRINT SUPPORT
    ///
 
    virtual UWorld* GetWorld() const override
    {
        if (HasAllFlags(RF_ClassDefaultObject))
        {
            // If we are a CDO, we must return nullptr instead of calling Outer->GetWorld()
            // to fool UObject::ImplementsGetWorld.
            return nullptr;
        }
        return GetOuter()->GetWorld();
    }
    
    /// The player will bark this text, and then exit the blueprint. Convenience function
    /// to use instead of a <code>BarkTask</code> when you don't care about waiting for the
    /// barking to finish.
    UFUNCTION(BlueprintCallable, Category = "VerbInteractions")
    void PlayerBarkAndEnd(FText Text);
    
    
    //////////////////////////////////
    ///
    /// GAME PLAY TAG ASSET
    ///
 
    virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;

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
