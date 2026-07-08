// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"
#include "AdventureGameplayTags.h"
#include "Item.h"
#include "Enums/ChoiceState.h"
#include "VerbType.h"
#include "Items/InventoryItem.h"
#include "UObject/Object.h"
#include "ItemManager.generated.h"

class UInventory;
class UItem;
class UItemTypeDefs;
class UItemSlot;
class UInventoryItem;

DECLARE_MULTICAST_DELEGATE(FUpdateItemInteractionText);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FActionDispatchDelegate, UStoryAction *, Action, UInventoryItem *, Item);

/**
 * Class to marshall all interactions and state of items in the game, including
 * in the player inventory, updating the HUD and updating the score when items
 * are collected. Note that persistent storage of the items and score are
 * delegated to the GameInstance.
 */
UCLASS(Blueprintable, BlueprintType, MinimalAPI)
class UItemManager : public UActorComponent
{
    GENERATED_BODY()
public:
	UItemManager();
	
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnInventoryChanged(FName ItemKind, EItemDisposition ItemDisposition);
	
	//////////////////////////////////
	///
	/// SCORE
	///
	
	UFUNCTION(BlueprintCallable, Category = "Player Actions")
	void AddToScore(int32 ScoreIncrement);
	
    //////////////////////////////////
	///
	/// ITEMS
	///
	
	/// Game asset reference for the table of item types in this game. Loaded at game
	/// launch type as required. Must be an instance of
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items")
	TSoftObjectPtr<UItemTypeDefs> ItemTypeDefinitions;
	
	/// Tell the UI to put the current verb and any current inventory item into the text display
	/// and if the InventoryItemInteraction is true, highlight the text.
	FUpdateItemInteractionText UpdateInventoryTextDelegate;

	/// Event for an action that should be handled.
	FActionDispatchDelegate ActionDispatch;
	
	void UpdateInventoryText();
	
	UFUNCTION(BlueprintCallable, Category = "Items")
	UItem *GetSourceItem() const;
	
	UFUNCTION(BlueprintCallable, Category = "Items")
	UItem *GetTargetItem() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items", Getter="GetSourceItemName")
	FName SourceItemName;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items", Getter="GetTargetItemName")
	FName TargetItemName;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items", Getter="GetSourceItemTag")
	FGameplayTag SourceItemTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items", Getter="GetTargetItemTag")
	FGameplayTag TargetItemTag;
	
private:
	void CheckForCustomInventoryItem(FName ItemDef);
	void CreateCustomInventoryItemHandler(FName ItemDef, UInventoryItem *InventoryItem);
	void CreateDefaultInventoryItem(FName ItemDef);
	
	/// Which item behaviours will be the <b>subject</b> of the current verb eg "Open Box"
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Items", meta=(AllowPrivateAccess=true))	
	UInventoryItem *SourceItem;

	/// Which item behaviours will the <b>object</b> of the current verb for
	/// example the door in "Use key on door". 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Items", meta=(AllowPrivateAccess=true))	
	UInventoryItem *TargetItem;
	
	/// Which item behaviours will be the <b>subject</b> of the current verb eg "Open Box"
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Items", meta=(AllowPrivateAccess=true))	
	UItem *Source;
	
	/// Which item behaviours will the <b>object</b> of the current verb for
	/// example the door in "Use key on door". 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Items", meta=(AllowPrivateAccess=true))	
	UItem *Target;
	
	FName GetSourceItemName() const { return Source ? Source->ItemTypeDef.GetTagLeafName() : NAME_None; }
	
	FName GetTargetItemName() const { return Target ? Target->ItemTypeDef.GetTagLeafName() : NAME_None; }

	FGameplayTag GetSourceItemTag() const { return Source ? Source->ItemTypeDef : AdventureGameplayTags::Item; }
	
	FGameplayTag GetTargetItemTag() const { return Target ? Target->ItemTypeDef : AdventureGameplayTags::Item; }

	bool GetHasSourceItem() const { return !!Source; }
	
	bool GetHasTargetItem() const { return !!Target; }
	
	TArray<EVerbType> ItemActionQueue;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items", Getter="GetHasSourceItem")
	bool HasSourceItem;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items", Getter="GetHasTargetItem")
	bool HasTargetItem;
	
	/// Item slot in the inventory either hovered or clicked.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Commands")
	UItemSlot *CurrentItemSlot = nullptr;

	/// Return true if the current source item can possibly ever interact with the
	/// <code>OtherItem</code>. Does <b>not</b> check the <code>InteractableItemName</code>
	/// field or any Story Action to see if an interaction exists.
	UFUNCTION(BlueprintCallable, Category = "Items")
	bool CanInteractWith(UPARAM(meta = (Categories="Item.Kind")) FGameplayTag OtherItem) const;
	
	/// Whether the subject of the verb is locked. Locked item choices won't change
	/// on mouse over of inventory. Default is <code>Unlocked</code>.
	///
	/// Set to locked when an item is clicked as part of a command. Set to unlocked
	/// when Source is set to null.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Items")	
	EChoiceState SourceLocked = EChoiceState::Unlocked;

	/// Whether the target of the verb is locked. Locked item choices won't change
	/// on mouse over of inventory. Default is <code>Unlocked</code>.
	///
	/// Set to locked when a target item is clicked as part of a Use or Give
	/// command. Set to unlocked when Target is set to null.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items")
	EChoiceState TargetLocked = EChoiceState::Unlocked;
	
	/// Set the source item to be the one in the given slot, and lock it; to prevent
	/// hover from changing it.
	/// @param SourceItemSlot The item slot to set and lock
	UFUNCTION(BlueprintCallable, Category = "Items")
	void SetAndLockSourceItem(UItemSlot *SourceItemSlot);
	
	/// Set the target item to be the one in the given slot, and lock it; to prevent
	/// hover from changing it.
	/// @param TargetItemSlot The item slot to set and lock
	UFUNCTION(BlueprintCallable, Category = "Items")
	void SetAndLockTargetItem(UItemSlot *TargetItemSlot);
	
	UFUNCTION(BlueprintCallable, Category = "Items")
	void SwapSourceAndTarget();
	
	/// Clears the <code>SourceItem</code> to null and sets it unlocked.
	UFUNCTION(Blueprintable)
	void ClearSourceItem()
	{
		SourceLocked = EChoiceState::Unlocked;
		if (SourceItem) SourceItem->ItemDetails = nullptr;
	}

	/// Clears the <code>SourceItem</code> to null and sets it unlocked.
	UFUNCTION(Blueprintable)
	void ClearTargetItem()
	{
		TargetLocked = EChoiceState::Unlocked;
		if (TargetItem) TargetItem->ItemDetails = nullptr;
	}
	
	UFUNCTION(BlueprintCallable, Category = "Items")
	void Reset()
	{
		ClearSourceItem();
		ClearTargetItem();
		CurrentItemSlot = nullptr;
	}
		
	//////////////////////////////////
	///
	/// INTERACTION
	///
	
	void ItemAddToInventory(const FName &ItemToAdd);

	void ItemRemoveFromInventory(const FName &ItemToRemove);

	void ItemsRemoveFromInventory(const TSet<FName> &SetOfItemsToRemove);
	
	void ItemRemoveFromInventoryAsync(const FName &ItemToRemove);

	void ItemsRemoveFromInventoryAsync(const TSet<FName> &ItemsToRemove);

private:
	TSet<FName> ItemsToRemove;
	
	UPROPERTY()
	UInventory *Inventory;

public:
	/// Handle a mouse click on an item button.
	bool MaybeHandleInventoryItemClicked(UItemSlot *ItemSlot);
	
	void MouseEnterInventoryItem(UItemSlot *ItemSlot);

	void MouseLeaveInventoryItem();

	/// Perform an interaction on the target item, ie via use or give.
	void PerformItemInteraction(EVerbType CurrentVerb);

	void PerformItemAction(EVerbType CurrentVerb);
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;
};
