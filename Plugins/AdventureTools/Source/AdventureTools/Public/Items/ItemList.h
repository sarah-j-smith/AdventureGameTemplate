// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"
#include "Enums/ItemDisposition.h"
#include "ItemKind.h"
#include "UObject/Object.h"
#include "Containers/List.h"

#include "ItemList.generated.h"

enum class EItemKind : uint8;
class UInventoryItem;

// https://unreal-garden.com/tutorials/delegates-advanced/#choosing-a-delegate-type

/**
 * Manage a list of items, for example as in a players inventory.
 *
 * This could also be used for a list of items in a loot drop, or quest or similar.
 */
UCLASS(Blueprintable, BlueprintType)
class ADVENTURETOOLS_API UItemList : public UObject
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
     * items in the middle which Linked lists are great at. The list
     * is also pretty good for traversal to find items meeting a
     * condition.
     *
     * Items are always added at the end. This is a little
     * expensive as we don't keep a tail pointer or backlinks. But its
     * still very fast even for dozens of items.
     *
     * One issue is that the TList::ElementType is a plain pointer
     * to UInventoryItem which is not recognised by the UE GC. To
     * make sure the item is retained, register it via
     * UGameInstance::RegisterReferencedObject
     */
    typedef TList<UInventoryItem*> FInventoryList;
    typedef TArray<UInventoryItem*> FInventoryArray;
    FInventoryList* Inventory = nullptr;

    /**
    * Low-level helper that adds the inventory item in the list, creating a
    * `FInventoryList` element to hold it.
     * @param InventoryItem UInventoryItem to add.
     */
    void AddItemToInventory(UInventoryItem* InventoryItem);

    /**
     * Low-level helper that deletes the element from the list and invalidates
     * any iterators. The @param Element will be an invalid pointer after this call.
     * @param Element FInventoryList Element to delete from the list and from memory.
     */
    void DeleteElementFromInventory(FInventoryList* Element);

    /// Debugging tool.
    void DumpInventoryToLog() const;

    void RegisterWithGameInstance(UInventoryItem* InventoryItem);

    void UnregisterFromGameInstance(UInventoryItem* InventoryItem);

public:
    DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnInventoryChangedSignature, FName /* Identifier */, EItemKind /* ItemKind */, EItemDisposition);
    FOnInventoryChangedSignature OnInventoryChanged;
    
    //////////////////////////////////
    ///
    /// CONFIGURATION
    ///

    /// In the case of multiple item lists this identifier distinguishes which one is updated.
    /// For example if used for a loot drop that depends on quest outcomes, this could be
    /// <code>MyQuestItemList->Identifier = "MyQuest_LootDrop"</code> to distinguish it from
    /// the players current inventory.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration")
    FName Identifier = "Inventory";

    /// Table of class references for creating instances of items.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UDataTable* InventoryDataTable;

#if WITH_EDITORONLY_DATA
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Testing")
    TSoftObjectPtr<UDataTable> InventoryDataTableTest;
#endif
    
    //////////////////////////////////
    ///
    /// ITEM REPORTING
    ///

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool Contains(EItemKind Item) const;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool IsEmpty() const;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    int InventorySize = 0;

    //////////////////////////////////
    ///
    /// ITEM MANAGEMENT
    ///

    /**
    * Add the given item to the current players inventory of held
    * items. Instantiates an <code>UInventoryItem</code> instance. The
    * instance will be a sub-class, with the actual class read from the
    * entry in the <b>InventoryDataTable</b>. 
    *
    * At present only one instance of any given <code>ItemKind</code> is
    * allowed to exist in the inventory at a time. This might change, but
    * for now its not supported to have more than one of anything.
    * 
    * @param ItemToAdd EItemKind to create an InventoryItem instance of. 
    * @return InventoryItem Created and added.
    */
    UFUNCTION(BlueprintCallable)
    UInventoryItem* AddItemToInventory(EItemKind ItemToAdd);

    /**
     * Removes the given item from the linked list used to track the inventory,
     * and sends the <code>OnInventoryChanged</code> event which should be used
     * by the owning pointers to the item to release it. Note that the entries in
     * the linked list are not UObject owning pointers. The owning pointers is
     * UItemSlot's InventoryItem UPROPERTY.  Also the ItemManager's SourceItem
     * TargetItem hold references for as long as the player is choosing an action.
     * These must all be set to null after receiving the OnInventoryChanged signal. 
     *  @param ItemToRemove EItemKind to remove an InventoryItem instance of.
     */
    UFUNCTION(BlueprintCallable)
    void RemoveItemKindFromInventory(EItemKind ItemToRemove);

    /**
     * Removes the given items from the current players inventory of held
     * items. Destroys the `UInventoryItem` instances of the class
     * from the `InventoryDataTable` and deletes them in the inventory UI
     * @param ItemsToRemove EItemKind set to remove an InventoryItem instances of.
     */
    UFUNCTION(BlueprintCallable)
    void RemoveItemKindsFromInventory(const TSet<EItemKind>& ItemsToRemove);

    /// Copy pointers to the current inventory into the given array out argument.
    /// @param Result Reference to an array of Inventory Item pointers. This array will be emptied and over-written.
    void GetInventoryItemsArray(TArray<UInventoryItem *> &Result) const;
};
