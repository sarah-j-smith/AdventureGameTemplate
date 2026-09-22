#pragma once

#include <CoreMinimal.h>

#include "ItemDisposition.h"

class UItem;
class UInventoryItem;

DECLARE_MULTICAST_DELEGATE_TwoParams(FInventoryChangedDelegate, FName, EItemDisposition);
DECLARE_MULTICAST_DELEGATE_TwoParams(FCustomInventoryItemLoaded, FName, UInventoryItem *);

struct ADVENTURETOOLS_API IInventoryManager
{
	virtual ~IInventoryManager() {};

	/**
	 * Perform one time initialisation after loading the class.
	 */
	virtual void Init() = 0;
	
	/**
	 * Creates and adds a new <code>UItem</code> into the inventory collection (if it was
	 * not already there), searching the inventory data table for the definition
	 * specified by the given <code>ItemKind`, then fires an event to notify the change.
	 * If the item is already there it does nothing.
	 * @param ItemKind The unique name of the item to add.
	 * @see FItemTypeDef
	 * @see UItemTypeDefs
	 */
	virtual void AddItemToInventory(FName ItemKind) = 0;

	/**
	 * Removes and destroys a <code>UItem</code> from the inventory collection (if it was
	 * there), then fires an event to notify the change. If the item is not in
	 * the inventory it does nothing.
	 * @param ItemKind The unique name of the item to remove.
	 */
	virtual void RemoveItemFromInventory(FName ItemKind) = 0;

	/**
	 * Removes and destroys a set of <code>UItem</code> instances from the inventory 
	 * collection (if each of them are there), then fires an event after each removal
	 * to notify the change. If none of the items are in the inventory it does nothing.
	 * @param ItemsToRemove The unique names of the items to remove.
	 */
	virtual void RemoveItemsFromInventory(const TSet<FName>& ItemsToRemove) = 0;

	/**
	 * Checks if a <code>UItem</code> is in the inventory. 
	 * @param ItemToCheck the unique name of the item to check for.
	 * @return true if the item is in the inventory, and false otherwise.
	 */
	virtual bool IsInInventory(const FName &ItemToCheck) const = 0;

	/**
	 * Gets a pointer to a <code>UItem</code> which is in the inventory, or null if it is not
	 * there.
	 * @param ItemToCheck the unique name of the item to get.
	 * @return A <code>UItem</code> pointer or <code>nullptr</code> if the item is not present.
	 */
	virtual UItem* GetItemFromInventory(const FName &ItemToCheck) = 0;

	/**
	 * Gets pointers to all the <code>UItem</code> instances in the inventory, by first emptying
	 * the given <code>Items</code> array, and then copying all the pointers into it. If the 
	 * inventory is empty, then <code>Items</code> will be an empty array after calling this.
	 * @param Items A modifiable reference to an array of <code>UItem</code> instances
	 * @warning Anything in <code>Items</code> will be <b>removed</b>
	 */
	virtual void GetInventoryItems(TArray<UItem*> &Items) = 0;

	/**
	 * Get a count of how many items are in the inventory.
	 * @return Integer count of how many items are in the inventory
	 */
	virtual int GetInventoryItemCount() const = 0;
	
	/**
	 * Try to load a custom <code>UInventoryItem</code> for the item with the given name.
	 * Fire an event once complete.
	 * @param ItemKind Item to load.
	 */
	virtual void GetCustomInventoryItem(FName ItemKind) = 0;

	/**
	 * Destroy the current inventory and rebuild it with fresh items from the list of
	 * names. This is mainly for use in load game and save game functions.
	 * @param InventoryKinds List of names to replace the current inventory with.
	 */
	virtual void RegenerateInventory(const TArray<FName> &InventoryKinds) = 0;

	/**
	 * Subscribe to be notified of changes to the inventory.  
	 * @param Subscriber A function that takes a delegate and binds to it to receive inventory change notifications.
	 */
	virtual void NotifyInventoryChanged(TFunctionRef<void(FInventoryChangedDelegate &)> Subscriber) = 0;
	
	/**
	 * Subscribe to be notified when a custom <code>UInventoryItem</code> is loaded.
	 * @param Subscriber A function that takes a delegate and binds to it to receive custom inventory loaded notifications.
	 */
	virtual void CustomInventoryItemLoaded(TFunctionRef<void(FCustomInventoryItemLoaded &)> Subscriber) = 0;
};

Expose_TNameOf(IInventoryManager)