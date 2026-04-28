// (c) 2025 Sarah Smith


#include "Items/ItemList.h"
#include "AdventureTools.h"
#include "Constants.h"
#include "Items/ItemData.h"
#include "Kismet/GameplayStatics.h"

bool UItemList::Contains(EItemKind Item) const
{
	for (const FInventoryList *Iterator = Inventory; Iterator; Iterator = Iterator->Next)
	{
		if (Iterator->Element->ItemKind == Item) return true;
	}
	return false;
}

bool UItemList::IsEmpty() const
{
	return Inventory == nullptr;
}

UInventoryItem *UItemList::AddItemToInventory(EItemKind ItemToAdd)
{
	if (ItemToAdd == EItemKind::None)
	{
		UE_LOG(LogAdventureGame, Warning, TEXT("Refusing to add EItemKind::None to inventory."));
		return nullptr;
	}
	const FItemData* ItemData = nullptr;
	const FName ItemName = FItemKind::GetUniqueName(ItemToAdd);
	if (const UDataTable *Dt = InventoryDataTable)
	{
		ItemData = Dt->FindRow<FItemData>(ItemName, "AddItemToInventory");
	}
	if (ItemData == nullptr)
	{
		UE_LOG(LogAdventureGame, Warning,
			TEXT("ItemData could not be loaded for name \"%s\". Check the InventoryDataTable set in ItemList."),
			*ItemName.ToString());
		return nullptr;
	}
	UInventoryItem* InventoryItem = NewObject<UInventoryItem>(this, ItemData->ItemClass, ItemName);
	FText NameText = FText::FromName(ItemName);
	FTextKey NameKey = ItemName.ToString();
	if (InventoryItem->Description.IsEmpty())
	{
		// The class blue print may specify a long description, don't over-write that
		const FText Description = FText::FromStringTable(ITEM_LONG_DESCRIPTIONS_KEY, NameKey);
		InventoryItem->Description = Description.IsEmpty() ? NameText : Description;
	}
	if (InventoryItem->ShortDescription.IsEmpty())
	{
		// The class blue print may specify a short description, don't over-write that
		const FText ShortDescription = FText::FromStringTable(ITEM_LONG_DESCRIPTIONS_KEY, NameKey);
		InventoryItem->ShortDescription = ShortDescription.IsEmpty() ? NameText : ShortDescription;
	}
	if (InventoryItem->ItemKind != ItemToAdd)
	{
		/// The class blueprint had the kind set to the wrong enum - this is an error.
		UE_LOG(LogAdventureGame, Error, TEXT("Item \"%s\": \"%s\" created from class with kind: %s - forcing to: %s"),
			*(ItemName.ToString()),
			*(InventoryItem->Description.ToString()),
			*(FItemKind::GetDescription(InventoryItem->ItemKind).ToString()),
			*(FItemKind::GetDescription(ItemToAdd).ToString())
		);
		InventoryItem->ItemKind = ItemToAdd;
	}
	AddItemToInventory(InventoryItem);
	OnInventoryChanged.Broadcast(Identifier, ItemToAdd, EItemDisposition::Added);
#if WITH_EDITOR
	DumpInventoryToLog();
#endif
	return InventoryItem;
}

void UItemList::RemoveItemKindFromInventory(EItemKind ItemToRemove)
{
	if (IsEmpty()) return;
	const TSet<EItemKind> Items({ItemToRemove});
	RemoveItemKindsFromInventory(Items);
}


void UItemList::RemoveItemKindsFromInventory(const TSet<EItemKind>& ItemsToRemove)
{
	if (IsEmpty()) return;
	FInventoryList *Item = Inventory;
	TSet RemovingItems(ItemsToRemove);
	TArray<EItemKind> ItemsThatWereRemoved;
	for (FInventoryList *Iterator = Inventory; Iterator; Iterator = Iterator->Next)
	{
		if (RemovingItems.Contains(Iterator->Element->ItemKind))
		{
			RemovingItems.Remove(Iterator->Element->ItemKind);
			DeleteElementFromInventory(Iterator);
			ItemsThatWereRemoved.Add(Iterator->Element->ItemKind);
			if (RemovingItems.Num() == 0) break;			
		}
	}
	if (RemovingItems.Num() > 0)
	{
		UE_LOG(LogAdventureGame, Warning, TEXT("Failed to remove some items from Inventory - %s"),
			*FItemKind::GetListDescription(ItemsToRemove.Array()));
	}
	for (const EItemKind ItemRemoved : ItemsThatWereRemoved)
	{
		OnInventoryChanged.Broadcast(Identifier, ItemRemoved, EItemDisposition::Removed);
	}
#if WITH_EDITOR
	DumpInventoryToLog();
#endif
}

void UItemList::GetInventoryItemsArray(TArray<UInventoryItem *> &Result) const
{
	Result.Empty();
	for (const FInventoryList *Iterator = Inventory; Iterator; Iterator = Iterator->Next)
	{
		Result.Add(Iterator->Element);
	}
}

void UItemList::DumpInventoryToLog() const
{
	unsigned int Index = 0;
	for (const FInventoryList *Iterator = Inventory; Iterator; Iterator = Iterator->Next)
	{
		FString Description = Iterator->Element->Description.ToString();
		UE_LOG(LogAdventureGame, Verbose, TEXT("   %d - %s"), Index++, *Description);
	}
}

void UItemList::RegisterWithGameInstance(UInventoryItem* InventoryItem)
{
	if (UGameInstance *GameInstance = UGameplayStatics::GetGameInstance(this))
	{
		GameInstance->RegisterReferencedObject(InventoryItem);
	}
}

void UItemList::UnregisterFromGameInstance(UInventoryItem* InventoryItem)
{
	if (UGameInstance *GameInstance = UGameplayStatics::GetGameInstance(this))
	{
		GameInstance->UnregisterReferencedObject(InventoryItem);
	}
}

void UItemList::AddItemToInventory(UInventoryItem* InventoryItem)
{
	if (UGameInstance *GameInstance = UGameplayStatics::GetGameInstance(this))
	{
		GameInstance->RegisterReferencedObject(InventoryItem);
	}
	FInventoryList *NewItem = new FInventoryList(InventoryItem);
	if (Inventory)
	{
		// list exists case - add it as the new tail
		FInventoryList *Tail = nullptr;
		for (FInventoryList* Iterator = Inventory; Iterator; Tail = Iterator, Iterator = Iterator->Next) {}
		Tail->Next = NewItem;
	}
	else
	{
		// list is empty case, item becomes the new head
		Inventory = NewItem;
	}
	InventorySize++;
}

void UItemList::DeleteElementFromInventory(FInventoryList* Element)
{
	if (UGameInstance *GameInstance = UGameplayStatics::GetGameInstance(this))
	{
		GameInstance->UnregisterReferencedObject(Element->Element);
	}
	if (!Element) return;
	bool Deleted = false;
	FInventoryList *Tmp = Inventory;
	if (Element == Inventory)
	{
		Inventory = Inventory->Next;
		delete Tmp;
		Deleted = true;
	}
	else
	{
		for (FInventoryList *It = Inventory; It; Tmp = It, It = It->Next)
		{
			if (It == Element)
			{
				Tmp->Next = It->Next;
				Deleted = true;
				delete It;
				break;
			}
		}
	}
	verify(Deleted);
	InventorySize--;
}