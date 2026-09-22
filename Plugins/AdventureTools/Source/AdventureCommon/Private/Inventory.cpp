// (c) 2026 Storybridge Games

#include "Inventory.h"
#include "Item.h"
#include "AdventureCommon.h"
#include "IItemTableProvider.h"
#include "ItemTypeDefs.h"
#include "Provider.h"

void FInventory::SetupHandlers()
{
	InventoryTableLoadCompleteDelegate.BindRaw(this, &FInventory::InventoryTableLoadCompleteHandler);
	ItemDetailsLoadCompleteDelegate.BindRaw(this, &FInventory::ItemDetailLoadCompleteHandler);
}

void FInventory::TearDownHandlers()
{
	if (InventoryTableLoadCompleteDelegate.IsBound()) InventoryTableLoadCompleteDelegate.Unbind();
	if (ItemDetailsLoadCompleteDelegate.IsBound()) ItemDetailsLoadCompleteDelegate.Unbind();
}

FInventory::FInventory()
	: ItemTableProvider(UProvider::Get()->GetInstance<IItemTableProvider>())
{
}

FInventory::~FInventory()
{
	TearDownHandlers();
}

void FInventory::AddItemToInventory(UItem* InventoryItem)
{
	if (!InventoryItem) return;
	FListOfItems *NewListElement = new FListOfItems(InventoryItem);
	if (Inventory)
	{
		// list exists case - add it as the new tail & move the tail ptr
		TailInventory->Next = NewListElement;
		TailInventory = NewListElement;
	}
	else
	{
		// list is empty case - add it as the new head & tail
		Inventory = NewListElement;
		TailInventory = NewListElement;
	}
	InventorySize++;
}

void FInventory::DeleteElementFromInventory(FListOfItems* Element)
{
	FListOfItems *Tmp = Inventory;
	if (Element == Inventory)
	{
		Inventory = Inventory->Next;
		InventorySize--;
		delete Tmp;
	}
	else
	{
		for (FListOfItems *It = Inventory; It; Tmp = It, It = It->Next)
		{
			if (It == Element)
			{
				Tmp->Next = It->Next;
				delete It;
				InventorySize--;
				break;
			}
		}
	}
}

void FInventory::DumpInventoryToLog() const
{
	unsigned int Index = 0;
	for (const FListOfItems *Iterator = Inventory; Iterator; Iterator = Iterator->Next)
	{
		FString Description = Iterator->Element->Description.ToString();
		UE_LOG(LogAdventureCommon, Verbose, TEXT("   %d - %s"), Index++, *Description);
	}
}

bool FInventory::Contains(const FName ItemName) const
{
	for (const FListOfItems *Iterator = Inventory; Iterator; Iterator = Iterator->Next)
	{
		if (Iterator->Element->ItemTypeDef.GetTagLeafName() == ItemName) return true;
	}
	return false;
}

UItem* FInventory::FindItemByName(FName ItemName) const
{
	for (const FListOfItems *Iterator = Inventory; Iterator; Iterator = Iterator->Next)
	{
		if (Iterator->Element->ItemTypeDef.GetTagLeafName() == ItemName) return Iterator->Element;
	}
	return nullptr;
}

void FInventory::AddItemInstanceByName(const FName ItemToAdd)
{
	if (Contains(ItemToAdd))
	{
		UE_LOG(LogAdventureCommon, Error, TEXT("already added"), *ItemToAdd.ToString());
		return;
	}
	if (FText OutReason; !ItemToAdd.IsValidObjectName(OutReason))
	{
		UE_LOG(LogAdventureCommon, Error, TEXT("Item name was invalid: %s"), *OutReason.ToString());
		return;
	}
	const auto InventoryDataTable = ItemTableProvider->GetItemDefinitionsTable();
	if (const UItemTypeDefs *Table = InventoryDataTable.Get())
	{
		AddNewItemToInventoryWithTable(ItemToAdd, Table);
		return;
	}
	UE_LOG(LogAdventureCommon, Log, TEXT("Async loading inventory data table: %s for %s"),
		*InventoryDataTable.ToString(), *ItemToAdd.ToString())
	TableOperationsQueue.Push(ItemToAdd);
	if (!Loading)
	{
		int32 _ = InventoryDataTable.LoadAsync(InventoryTableLoadCompleteDelegate);
	}
}

void FInventory::InventoryTableLoadCompleteHandler(const FSoftObjectPath& Path, UObject* Object)
{
	const UItemTypeDefs* Table = ItemTableProvider->GetItemDefinitionsTable().Get();
	ensureAlwaysMsgf(Table, TEXT("InventoryTableLoadCompleteHandler: Table not valid"));
	while (!TableOperationsQueue.IsEmpty())
	{
		const FName ItemName = TableOperationsQueue.Pop();
		AddNewItemToInventoryWithTable(ItemName, Table);
	}
}

void FInventory::AddNewItemToInventoryWithTable(FName ItemName, const UItemTypeDefs *Table)
{
	const FItemTypeDef ItemTypeDef = Table->FindDefByName(ItemName);
	if (!ItemTypeDef.bValid)
	{
		UE_LOG(LogAdventureCommon, Error, TEXT("Item name was invalid: %s"), *ItemName.ToString());
		OnInventoryChanged.Broadcast(ItemName, EItemDisposition::Error);
		return;
	}
	if (UItem *ItemDetails = ItemTypeDef.Item.Get())
	{
		AddNewItemToInventoryWithDetails(ItemDetails, ItemTypeDef.UniqueName.GetTagLeafName());
		return;
	}
	if (ItemTypeDef.Item.IsPending())
	{
		// It's been set, just needs loading
		int32 _ = ItemTypeDef.Item.LoadAsync(ItemDetailsLoadCompleteDelegate);
		return;
	}
	OnInventoryChanged.Broadcast(ItemName, EItemDisposition::Error);
	UE_LOG(LogAdventureCommon, Warning,TEXT("ItemClass missing in ItemTypeDef for: %s"), *ItemName.ToString());
}

void FInventory::ItemDetailLoadCompleteHandler(const FSoftObjectPath& Path, UObject* /* Object */)
{
	const UItemTypeDefs *Table = ItemTableProvider->GetItemDefinitionsTable().Get();
	ensureAlwaysMsgf(Table, TEXT("ItemDetailLoadCompleteHandler: Error, expected table to be loaded!"));
	const TSoftObjectPtr<UItem> ItemDetailPtr(Path);
	UItem *ItemDetails = ItemDetailPtr.Get();
	ensureAlwaysMsgf(ItemDetails, TEXT("Details expected once loaded %s"), *Path.GetAssetPathString());
	const FItemTypeDef ItemTypeDef = Table->FindDefByName(ItemDetails->ItemTypeDef.GetTagLeafName());
	ensureAlwaysMsgf(ItemTypeDef.bValid, TEXT("ItemDetailLoadCompleteHandler: type def must exist in table!"));
	AddNewItemToInventoryWithDetails(ItemDetails, ItemTypeDef.UniqueName.GetTagLeafName());
}

void FInventory::AddNewItemToInventoryWithDetails(UItem *ItemDetails, FName ItemName)
{
	AddItemToInventory(ItemDetails);
	OnInventoryChanged.Broadcast(ItemName, EItemDisposition::Added);
}

void FInventory::RemoveItemInstanceByName(const FName ItemToRemove)
{
	if (IsEmpty()) return;
	for (FListOfItems *Iterator = Inventory; Iterator; Iterator = Iterator->Next)
	{
		if (ItemToRemove == Iterator->Element->ItemTypeDef.GetTagLeafName())
		{
			OnInventoryChanged.Broadcast(ItemToRemove, EItemDisposition::Removed);
			ItemTableProvider->UnregisterFromGameInstance(Iterator->Element);
			DeleteElementFromInventory(Iterator);
			return;
		}
	}
}

void FInventory::RemoveItemKindsFromInventory(const TSet<FName>& ItemsToRemove)
{
	if (IsEmpty()) return;
	if (ItemsToRemove.IsEmpty()) return;
	for (const FName ItemToRemove : ItemsToRemove)
	{
		RemoveItemInstanceByName(ItemToRemove);
	}
}

void FInventory::GetInventoryItemsArray(TArray<UItem*>& Result) const
{
	Result.Empty();
	for (const FListOfItems *Iterator = Inventory; Iterator; Iterator = Iterator->Next)
	{
		Result.Add(Iterator->Element);
	}
}
