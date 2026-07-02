// (c) 2026 Storybridge Games


#include "Inventory.h"
#include "Item.h"
#include "AdventureCommon.h"
#include "ItemTypeDefs.h"
#include "Kismet/GameplayStatics.h"

void UInventory::SetupHandlers()
{
	InventoryTableLoadCompleteDelegate.BindUObject(this, &UInventory::InventoryTableLoadCompleteHandler);
	ItemDetailsLoadCompleteDelegate.BindUObject(this, &UInventory::ItemDetailLoadCompleteHandler);
}

void UInventory::TearDownHandlers()
{
	if (InventoryTableLoadCompleteDelegate.IsBound()) InventoryTableLoadCompleteDelegate.Unbind();
	if (ItemDetailsLoadCompleteDelegate.IsBound()) ItemDetailsLoadCompleteDelegate.Unbind();
}

void UInventory::AddItemToInventory(UItem* InventoryItem)
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

void UInventory::DeleteElementFromInventory(FListOfItems* Element)
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

void UInventory::DumpInventoryToLog() const
{
	unsigned int Index = 0;
	for (const FListOfItems *Iterator = Inventory; Iterator; Iterator = Iterator->Next)
	{
		FString Description = Iterator->Element->Description.ToString();
		UE_LOG(LogAdventureCommon, Verbose, TEXT("   %d - %s"), Index++, *Description);
	}
}

void UInventory::RegisterWithGameInstance(UItem* InventoryItem)
{
	if (UGameInstance *GameInstance = UGameplayStatics::GetGameInstance(this))
	{
		GameInstance->RegisterReferencedObject(InventoryItem);
	}
}

void UInventory::UnregisterFromGameInstance(UItem* InventoryItem)
{
	if (UGameInstance *GameInstance = UGameplayStatics::GetGameInstance(this))
	{
		GameInstance->UnregisterReferencedObject(InventoryItem);
	}
}

bool UInventory::Contains(const FName ItemName) const
{
	for (const FListOfItems *Iterator = Inventory; Iterator; Iterator = Iterator->Next)
	{
		if (Iterator->Element->ItemTypeDef == ItemName) return true;
	}
	return false;
}

UItem* UInventory::FindItemByName(FName ItemName) const
{
	for (const FListOfItems *Iterator = Inventory; Iterator; Iterator = Iterator->Next)
	{
		if (Iterator->Element->ItemTypeDef == ItemName) return Iterator->Element;
	}
	return nullptr;
}

void UInventory::AddItemInstanceByName(FName ItemToAdd)
{
	FText OutReason;
	if (Contains(ItemToAdd) || !ItemToAdd.IsValidObjectName(OutReason))
	{
		UE_LOG(LogAdventureCommon, Error, TEXT("Item name was invalid: %s"), OutReason.IsEmpty() ? TEXT("already added") : *OutReason.ToString());
		return;
	}
	if (const UItemTypeDefs *Table = InventoryDataTable.Get())
	{
		AddNewItemToInventoryWithTable(ItemToAdd, Table);
		return;
	}
	TableOperationsQueue.Push(ItemToAdd);
	if (!Loading)
	{
		int32 _ = InventoryDataTable.LoadAsync(InventoryTableLoadCompleteDelegate);
	}
}

void UInventory::InventoryTableLoadCompleteHandler(const FSoftObjectPath& Path, UObject* Object)
{
	UItemTypeDefs* Table = InventoryDataTable.Get();
	ensureAlwaysMsgf(Table, TEXT("InventoryTableLoadCompleteHandler: Table not valid"));
	while (!TableOperationsQueue.IsEmpty())
	{
		const FName ItemName = TableOperationsQueue.Pop();
		AddNewItemToInventoryWithTable(ItemName, Table);
	}
}

void UInventory::AddNewItemToInventoryWithTable(FName ItemName, const UItemTypeDefs *Table)
{
	const FItemTypeDef ItemTypeDef = Table->FindDefByName(ItemName);
	if (!ItemTypeDef.Item.IsValid())
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

void UInventory::ItemDetailLoadCompleteHandler(const FSoftObjectPath& Path, UObject* /* Object */)
{
	const UItemTypeDefs *Table = InventoryDataTable.Get();
	ensureAlwaysMsgf(Table, TEXT("ItemDetailLoadCompleteHandler: Error, expected table to be loaded!"));
	TSoftObjectPtr<UItem> ItemDetailPtr(Path);
	UItem *ItemDetails = ItemDetailPtr.Get();
	ensureAlwaysMsgf(ItemDetails, TEXT("Details expected once loaded %s"), *Path.GetAssetPathString());
	const FItemTypeDef ItemTypeDef = Table->FindDefByName(ItemDetails->ItemTypeDef);
	ensureAlwaysMsgf(ItemTypeDef.bValid, TEXT("ItemDetailLoadCompleteHandler: type def must exist in table!"));
	AddNewItemToInventoryWithDetails(ItemDetails, ItemTypeDef.UniqueName.GetTagLeafName());
}

void UInventory::AddNewItemToInventoryWithDetails(UItem *ItemDetails, FName ItemName)
{
	AddItemToInventory(ItemDetails);
	OnInventoryChanged.Broadcast(ItemName, EItemDisposition::Added);
}

void UInventory::RemoveItemInstanceByName(const FName ItemToRemove)
{
	if (IsEmpty()) return;
	for (FListOfItems *Iterator = Inventory; Iterator; Iterator = Iterator->Next)
	{
		if (ItemToRemove == Iterator->Element->ItemTypeDef)
		{
			OnInventoryChanged.Broadcast(ItemToRemove, EItemDisposition::Removed);
			UnregisterFromGameInstance(Iterator->Element);
			DeleteElementFromInventory(Iterator);
			return;
		}
	}
}

void UInventory::RemoveItemKindsFromInventory(const TSet<FName>& ItemsToRemove)
{
	if (IsEmpty()) return;
	if (ItemsToRemove.IsEmpty()) return;
	for (const FName ItemToRemove : ItemsToRemove)
	{
		RemoveItemInstanceByName(ItemToRemove);
	}
}

void UInventory::GetInventoryItemsArray(TArray<UItem*>& Result) const
{
	Result.Empty();
	for (const FListOfItems *Iterator = Inventory; Iterator; Iterator = Iterator->Next)
	{
		Result.Add(Iterator->Element);
	}
}
