#pragma once

#include <CoreMinimal.h>

#include "IInventoryManager.h"
#include "IItemTableProvider.h"
#include "ItemDisposition.h"

#include "InventoryManager.generated.h"

class FInventory;
class UInventoryItem;

#define PLAYER_INVENTORY_NAME "PlayerInventory"

UCLASS()
class UInventoryManager : public UObject, public IInventoryManager
{
	GENERATED_UCLASS_BODY()
	
	UInventoryManager();
	
	virtual ~UInventoryManager() override = default;

	virtual void Init() override;
	
	virtual void AddItemToInventory(FName ItemKind) override;

	virtual void RemoveItemFromInventory(FName ItemKind) override;

	virtual void RemoveItemsFromInventory(const TSet<FName>& ItemsToRemove) override;

	virtual bool IsInInventory(const FName& ItemToCheck) const override;

	virtual UItem* GetItemFromInventory(const FName& ItemToCheck) override;

	virtual void GetInventoryItems(TArray<UItem*>& Items) override;

	virtual int GetInventoryItemCount() const override;
	
	/// Try to load a custom <code>UInventoryItem</code> for the item with the given name.
	virtual void GetCustomInventoryItem(FName ItemKind) override;
	
	virtual void RegenerateInventory(const TArray<FName> &InventoryKinds) override;
	
	virtual void NotifyInventoryChanged(TFunctionRef<void(FInventoryChangedDelegate&)> Subscriber) override;
	
	virtual void CustomInventoryItemLoaded(TFunctionRef<void(FCustomInventoryItemLoaded&)> Subscriber) override;
private:
	TSharedPtr<FInventory> Inventory;
	
	FLoadSoftObjectPathAsyncDelegate LoadTableDelegate;
	FLoadSoftObjectPathAsyncDelegate LoadClassDelegate;

	FInventoryChangedDelegate OnInventoryChanged;
	FCustomInventoryItemLoaded CustomInventoryItemLoadedDelegate;
	FDelegateHandle OnInventoryChangedHandle;

	void CreateInventory();

	void DestroyInventory();

	void BindInventoryChangedHandlers();

	void InventoryChanged(FName ItemKind, EItemDisposition ItemDisposition);

	void InventoryTableLoadCompleteHandler(const FSoftObjectPath& Path, UObject* Object);

	void InventoryClassLoadCompleteHandler(const FSoftObjectPath& Path, UObject* Object);

	void GetCustomInventoryItemWithTable(FName ItemKind, UDataTable* DataTablePtr);

	void GetCustomInventoryItemWithClass(FName ItemKind, const UClass* InventoryItemClass);

	TArray<FName> TableOperationsQueue;
	TMap<FString, FName> ClassOperationsQueue;

	TSharedPtr<IItemTableProvider> ItemTableProvider;
};
