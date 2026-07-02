// (c) 2026 Storybridge Games

#pragma once

#include "CoreMinimal.h"
#include "ItemDisposition.h"
#include "UObject/Object.h"
#include "ItemTypeDef.h"

#include "Inventory.generated.h"

class UItemTypeDefs;
class UItem;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnInventoryChanged, FName /* Identifier */, EItemDisposition /* Identifier */);

/**
 * Manage a list of items, for example as in a players inventory.
 *
 * This could also be used for a list of items in a loot drop, or quest or similar.
 */
UCLASS()
class ADVENTURECOMMON_API UInventory : public UObject
{
	GENERATED_BODY()

protected:
	/**
	 * Use a Linked List here because the TArray throws exceptions
	 * when you remove data from the middle of it due to the way it
	 * tries to preserve the order of items, shuffling them up to
	 * cover removed items.
	 *
	 * The semantics of the Inventory are that it
	 * is an ordered list, not very long, and we often want to remove
	 * items in the middle which Linked lists are great at. The linked list
	 * is also pretty good for traversal to find items meeting a
	 * condition.
	 *
	 * Items are always added at the end. For this reason we keep a 
	 * tail pointer as an optimisation. There are no back-pointers.
	 *
	 * One issue is that the TList::ElementType is a plain pointer
	 * to UItem which is not recognised by the UE GC. To
	 * make sure the item is retained, register it via
	 * UGameInstance::RegisterReferencedObject
	 */
	typedef TList<UItem*> FListOfItems;
	typedef TArray<UItem*> FArrayOfItems;
	FListOfItems* Inventory = nullptr;
	FListOfItems* TailInventory = nullptr;
	
	int InventorySize = 0;
	
	/**
	 * Low-level helper that adds the inventory item in the list, creating a
	 * `FListOfItems` element to hold it.
	 * @param InventoryItem UItem to add.
	 */
	void AddItemToInventory(UItem* InventoryItem);

	/**
	 * Low-level helper that deletes the element from the list and invalidates
	 * any iterators. The @param Element will be an invalid pointer after this call.
	 * @param Element FInventoryList Element to delete from the list and from memory.
	 */
	void DeleteElementFromInventory(FListOfItems* Element);

	/// Debugging tool.
	void DumpInventoryToLog() const;

	void RegisterWithGameInstance(UItem* InventoryItem);

	void UnregisterFromGameInstance(UItem* InventoryItem);
	
public:
	/// Bind to this delegate to be advised when this inventory changes by 
	/// removing or adding items.
	FOnInventoryChanged OnInventoryChanged;
	
	/// Bind the inventory's internal table and class loading complete handlers.
	void SetupHandlers();

	/// Bind the inventory's internal table and class loading complete handlers.
	void TearDownHandlers();

	//////////////////////////////////
	///
	/// CONFIGURATION
	///

	/// Identifier for this inventory of items. If it was a separate list of items
	/// for a loot drop, as opposed to the default (PlayerInventory) then it could be
	/// for example "MyQuest_LootDrop"
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration")
	FName Identifier = "PlayerInventory";

	/// Table of class references for creating instances of items.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Configuration")
	TSoftObjectPtr<UItemTypeDefs> InventoryDataTable;

private:
	FLoadSoftObjectPathAsyncDelegate InventoryTableLoadCompleteDelegate;
	FLoadSoftObjectPathAsyncDelegate ItemDetailsLoadCompleteDelegate;
	
	UFUNCTION()
	void InventoryTableLoadCompleteHandler(const FSoftObjectPath& Path, UObject* Object);
	
	UFUNCTION()
	void ItemDetailLoadCompleteHandler(const FSoftObjectPath& Path, UObject* Object);
	
	bool Loading = false;
	
	/// Queue of all operations waiting for a valid table
	TArray<FName> TableOperationsQueue;
	
	void AddNewItemToInventoryWithTable(FName ItemName, const UItemTypeDefs *Table);
	
	void AddNewItemToInventoryWithDetails(UItem *ItemDetails, FName ItemName);
	
public:
	//////////////////////////////////
    ///
    /// ITEM REPORTING
    ///
	
	/// Test if the item identified by the given unique Item if in this inventory.
	/// @param ItemName Unique name to search for
	/// @returns true if the item is present and false otherwise
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool Contains(FName ItemName) const;

	/// Find and return an item from the inventory with the given name, or `nullptr`
	/// if the item is not present.
	/// @param ItemName FName of the object to fetch
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	UItem *FindItemByName(FName ItemName) const;
	
	/// Test if this inventory is currently empty.
	/// @returns true if it contains no items
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool IsEmpty() const { return InventorySize == 0; };

	/// Count of items in the inventory.
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    int GetInventorySize() const { return InventorySize; };

    //////////////////////////////////
    ///
    /// ITEM MANAGEMENT
    ///

    /**
    * Add the named item to the inventory. Instantiates an <code>UItem</code>
    * instance. The instance will be a subclass of UItem, with the actual class 
    * read from the entry in the <b>InventoryDataTable</b>. 
    *
    * Throws a fatal error if the item is already in the inventory. Note that
    * async loading of the 
    * 
    * @param ItemToAdd FName to create an InventoryItem instance of. 
    */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void AddItemInstanceByName(FName ItemToAdd);

    /**
     * Removes the given item from the inventory. 
     *  @param ItemToRemove FName to remove an InventoryItem instance of.
     */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void RemoveItemInstanceByName(FName ItemToRemove);

    /**
     * Removes the given items from the current players inventory of held
     * items. Destroys the `UItem` instances of the class
     * from the `InventoryDataTable` and deletes them in the inventory UI
     * @param ItemsToRemove EItemKind set to remove an InventoryItem instances of.
     */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void RemoveItemKindsFromInventory(const TSet<FName>& ItemsToRemove);

    /// Copy pointers to the current inventory into the given array out argument.
    /// @param Result Reference to an array of Inventory Item pointers. This array will be emptied and over-written.
    void GetInventoryItemsArray(TArray<UItem *> &Result) const;
};
