// (c) 2026 Storybridge Games


#include "Inventory.h"
#include "Item.h"
#include "AdventureCommon.h"
#include "Constants.h"
#include "ItemTypeDefs.h"
#include "Kismet/GameplayStatics.h"

void UInventory::SetupHandlers()
{
	InventoryTableLoadCompleteDelegate.BindUObject(this, &UInventory::InventoryTableLoadCompleteHandler);
	ItemClassLoadCompleteDelegate.BindUObject(this, &UInventory::ItemClassLoadCompleteHandler);
}

void UInventory::TearDownHandlers()
{
	if (InventoryTableLoadCompleteDelegate.IsBound()) InventoryTableLoadCompleteDelegate.Unbind();
	if (ItemClassLoadCompleteDelegate.IsBound()) ItemClassLoadCompleteDelegate.Unbind();
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

void UInventory::AddItemInstanceByName(const FName ItemToAdd)
{
	if (Contains(ItemToAdd))
	{
		const FString ErrorMessage = FString::Printf(TEXT("AddToInventory: item already in the inventory: %s"),*ItemToAdd.ToString());
		UE_LOG(LogAdventureCommon, Error, TEXT("%s"), *ErrorMessage);
#if WITH_EDITOR
		GEngine->AddOnScreenDebugMessage(INVENTORY_MISSING_DEBUG_KEY, 10.0, FColor::Red,
										 *ErrorMessage, false, FVector2D(2.0, 2.0));
#endif
		return;
	}
	FText OutReason;
	if (!ItemToAdd.IsValidObjectName(OutReason))
	{
		const FString ErrorMessage = FString::Printf(TEXT("Item name was invalid: %s"), *OutReason.ToString());
		UE_LOG(LogAdventureCommon, Error, TEXT("%s"), *ErrorMessage);
#if WITH_EDITOR
		GEngine->AddOnScreenDebugMessage(INVENTORY_MISSING_DEBUG_KEY, 10.0, FColor::Red,
										 *ErrorMessage, false, FVector2D(2.0, 2.0));
#endif
		return;
	}
	if (const UItemTypeDefs *Table = InventoryDataTable.Get())
	{
		AddNewItemToInventory(ItemToAdd, Table);
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
	FString LevelName = "Unknown level";
	if (const UItemTypeDefs *Items = Cast<UItemTypeDefs>(Object))
	{
		// OK, we have a valid table now, lets finish the work
		while (!TableOperationsQueue.IsEmpty())
		{
			const FName Request = TableOperationsQueue.Pop();
			AddNewItemToInventory(Request, Items);
		}
		return;
	}
	const FString ErrorMessage = FString::Printf(TEXT("InventoryDataTable property missing in Inventory"));
#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(INVENTORY_MISSING_DEBUG_KEY, 10.0, FColor::Red,
									 *ErrorMessage, false, FVector2D(2.0, 2.0));
#endif
	UE_LOG(LogAdventureCommon, Error, TEXT("SetupAIController error. %s"), *ErrorMessage);
}

void UInventory::AddNewItemToInventory(FName ItemName, const UItemTypeDefs *Table)
{
	const FItemTypeDef ItemTypeDef = Table->FindDefByName(ItemName);
	if (!ItemTypeDef.bValid)
	{
		const FString ErrorMessage = FString::Printf(TEXT("InventoryDataTable property missing in Inventory"));
#if WITH_EDITOR
		GEngine->AddOnScreenDebugMessage(INVENTORY_MISSING_DEBUG_KEY, 10.0, FColor::Red,
										 *ErrorMessage, false, FVector2D(2.0, 2.0));
#endif
		UE_LOG(LogAdventureCommon, Warning,
			TEXT("ItemTypeDef could not be loaded for name \"%s\". Check the InventoryDataTable set in ItemList."),
			*ItemName.ToString());
		return;
	}
	if (UClass *ItemClass = ItemTypeDef.ItemClass.Get())
	{
		AddNewItemToInventoryWithClass(ItemClass, ItemTypeDef.UniqueName.GetTagLeafName());
		return;
	}
	// Either needs to be loaded, or game designer forgot to set this
	if (ItemTypeDef.ItemClass.IsPending())
	{
		// It's been set, just needs loading
		ItemOperationsQueue.Push(ItemName);
		int32 _ = ItemTypeDef.ItemClass.LoadAsync(ItemClassLoadCompleteDelegate);
		return;
	}
	// Forgot to set!
	const FString ErrorMessage = FString::Printf(
		TEXT("ItemClass missing in Inventory ItemTypeDef for: %s"),
		*ItemName.ToString());
#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(INVENTORY_MISSING_DEBUG_KEY, 10.0, FColor::Red,
									 *ErrorMessage, false, FVector2D(2.0, 2.0));
#endif
	UE_LOG(LogAdventureCommon, Warning,TEXT("%s"), *ErrorMessage);
}

void UInventory::ItemClassLoadCompleteHandler(const FSoftObjectPath& Path, UObject* Object)
{
	const UItemTypeDefs *Table = InventoryDataTable.Get();
	ensureAlwaysMsgf(Table, TEXT("Must not be calling the item class load handler if the table is not loaded!"));
	TSoftClassPtr<UItem> ItemClassPtr(Path);
	if (const UClass *ItemClass = ItemClassPtr.Get())
	{
		const FItemTypeDef ItemTypeDef = Table->FindItemByClass(ItemClass->GetName());
		AddNewItemToInventoryWithClass(ItemClass, ItemTypeDef.UniqueName.GetTagLeafName());
	}
}

void UInventory::AddNewItemToInventoryWithClass(const UClass *ItemClass, FName ItemName)
{
	UItem* InventoryItem = NewObject<UItem>(this, ItemClass, ItemName);
	RegisterWithGameInstance(InventoryItem);
	AddItemToInventory(InventoryItem);
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
