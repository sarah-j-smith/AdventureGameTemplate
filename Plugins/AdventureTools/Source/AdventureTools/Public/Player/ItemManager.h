// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"
#include "Enums/ChoiceState.h"
#include "ItemKind.h"
#include "VerbType.h"
#include "UObject/Object.h"
#include "ItemManager.generated.h"

class UItemSlot;
class UInventoryItem;
class UAdventureGameInstance;

DECLARE_MULTICAST_DELEGATE(FUpdateItemInteractionText);

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

	/// Tell the UI to put the current verb and any current inventory item into the text display
	/// and if the InventoryItemInteraction is true, highlight the text.
	FUpdateItemInteractionText UpdateInventoryTextDelegate;

	void UpdateInventoryText();
	
	/// Which item will be the <b>subject</b> of the current verb eg "Open Box"
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Commands")	
	UInventoryItem *SourceItem;

	/// Which item will the <b>object</b> of the current verb for
	/// example the door in "Use key on door"
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items")
	UInventoryItem *TargetItem;

	/// Item slot in the inventory either hovered or clicked.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Commands")
	UItemSlot *CurrentItemSlot = nullptr;

	bool CanInteractWith(EItemKind OtherItem) const;
	
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
	
	UFUNCTION(BlueprintCallable, Category = "Items")
	void SwapSourceAndTarget();

	UFUNCTION(BlueprintCallable, Category = "Items")
	void SetAndLockSourceItem(UInventoryItem *Item)
	{
		SourceItem = Item;
		SourceLocked = EChoiceState::Locked;
	}
	
	UFUNCTION(BlueprintCallable, Category = "Items")
	void SetAndLockTargetItem(UInventoryItem *Item)
	{
		TargetItem = Item;
		TargetLocked = EChoiceState::Locked;
	}
	
	/// Clears the <code>SourceItem</code> to null and sets it unlocked.
	UFUNCTION(Blueprintable)
	void ClearSourceItem()
	{
		SourceLocked = EChoiceState::Unlocked;
		SourceItem = nullptr;
	}

	/// Clears the <code>SourceItem</code> to null and sets it unlocked.
	UFUNCTION(Blueprintable)
	void ClearTargetItem()
	{
		TargetLocked = EChoiceState::Unlocked;
		TargetItem = nullptr;
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
	
	UInventoryItem *ItemAddToInventory(const EItemKind &ItemToAdd);

	void ItemRemoveFromInventory(const EItemKind &ItemToRemove);

	void ItemsRemoveFromInventory(const TSet<EItemKind> &ItemsToRemove);
	
	void ItemRemoveFromInventoryAsync(const EItemKind &ItemToRemove);

	void ItemsRemoveFromInventoryAsync(const TSet<EItemKind> &ItemsToRemove);

private:
	UAdventureGameInstance *GetAdventureGameInstance();

	TSet<EItemKind> ItemsToRemove;
	
public:
	/// Handle a mouse click on an item button.
	bool MaybeHandleInventoryItemClicked(UItemSlot *ItemSlot);
	
	void MouseEnterInventoryItem(UItemSlot *ItemSlot);

	void MouseLeaveInventoryItem();

	/// Perform an interaction on the target item, ie via use or give.
	void PerformItemInteraction(EVerbType CurrentVerb);

	void PerformItemAction(EVerbType CurrentVerb);

	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;
};
