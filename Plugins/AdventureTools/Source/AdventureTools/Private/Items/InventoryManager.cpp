#include "Items/InventoryManager.h"

#include "Inventory.h"
#include "Provider.h"
#include "AdventureTools.h"
#include "IItemTableProvider.h"
#include "Items/InventoryItem.h"
#include "Items/ItemData.h"

UInventoryManager::UInventoryManager(FObjectInitializer const &Initializer)
	: UObject(Initializer)
	, ItemTableProvider(UProvider::Get()->GetInstance<IItemTableProvider>())
{
}

UInventoryManager::UInventoryManager()
	: ItemTableProvider(UProvider::Get()->GetInstance<IItemTableProvider>())
{
}

void UInventoryManager::Init()
{
	CreateInventory();
	BindInventoryChangedHandlers();
	
	LoadTableDelegate.BindUObject(this, &UInventoryManager::InventoryTableLoadCompleteHandler);
	LoadClassDelegate.BindUObject(this, &UInventoryManager::InventoryClassLoadCompleteHandler);
}

void UInventoryManager::InventoryChanged(FName ItemKind,
											  EItemDisposition ItemDisposition)
{
	OnInventoryChanged.Broadcast(ItemKind, ItemDisposition);
}

void UInventoryManager::InventoryTableLoadCompleteHandler(const FSoftObjectPath& Path, UObject* Object)
{
	UE_LOG(LogAdventureGame, VeryVerbose, TEXT("AdventureGameInstance::InventoryTableLoadCompleteHandler: %s - %s"), 
	*Path.ToString(),
	*(Object ? Object->GetName() : FString(TEXT("NULL OBJECT"))));
	const TSoftObjectPtr<UDataTable> DataTablePtr(Path);
	ensureAlwaysMsgf(DataTablePtr.IsValid(), TEXT("InventoryTableLoadCompleteHandler: DataTable is not valid"));
	while (!TableOperationsQueue.IsEmpty())
	{
		const FName ItemName = TableOperationsQueue.Pop();
		GetCustomInventoryItemWithTable(ItemName, DataTablePtr.Get());
	}
}

void UInventoryManager::InventoryClassLoadCompleteHandler(const FSoftObjectPath& Path, UObject* Object)
{
	UE_LOG(LogAdventureGame, VeryVerbose, TEXT("AdventureGameInstance::InventoryClassLoadCompleteHandler: %s - %s"), 
	*Path.ToString(),
	*(Object ? Object->GetName() : FString(TEXT("NULL OBJECT"))));
	const TSoftClassPtr<UInventoryItem> InventoryItemClassPtr(Path);
	const UClass* InventoryItemClass = InventoryItemClassPtr.Get();
	ensureAlwaysMsgf(InventoryItemClass, TEXT("InventoryClassLoadCompleteHandler: InventoryItemClass is not valid"));
	const FName ItemName = ClassOperationsQueue.FindAndRemoveChecked(Path.ToString());
	GetCustomInventoryItemWithClass(ItemName, InventoryItemClass);
}

void UInventoryManager::GetCustomInventoryItemWithTable(FName ItemKind, UDataTable* DataTablePtr)
{
	ensureAlwaysMsgf(DataTablePtr, TEXT("GetCustomInventoryItemWithTable: Error, expected table to be loaded"));
	const auto ItemBehavioursTable = ItemTableProvider->GetItemBehavioursTable();
	const FItemData *ItemRow  = ItemBehavioursTable->FindRow<FItemData>(ItemKind, "GetCustomInventoryItemWithTable");
	if (ItemRow == nullptr)
	{
		/// There is no custom UInventoryItem for this ItemName
		CustomInventoryItemLoadedDelegate.Broadcast(ItemKind, nullptr);
	}
	if (const UClass *InventoryItemClass = ItemRow->ItemClass.Get())
	{
		GetCustomInventoryItemWithClass(ItemKind, InventoryItemClass);
		return;
	}
	const FSoftObjectPath ItemClassPath = ItemRow->ItemClass.ToSoftObjectPath();
	ClassOperationsQueue.Add(ItemClassPath.ToString(), ItemKind);
	int32 _ = ItemRow->ItemClass.LoadAsync(LoadClassDelegate);
}

void UInventoryManager::GetCustomInventoryItemWithClass(FName ItemKind, const UClass* InventoryItemClass)
{
	UInventoryItem *InventoryItem = NewObject<UInventoryItem>(this, InventoryItemClass, ItemKind);
	
	// YU ask for custom item and then not stick around to get your order? Should call CustomInventoryItemLoaded!
	ensureAlwaysMsgf(CustomInventoryItemLoadedDelegate.IsBound(), 
		TEXT("GetCustomInventoryItemWithClass: CustomInventoryItem is not bound - forgot to call CustomInventoryItemLoaded?"));
	CustomInventoryItemLoadedDelegate.Broadcast(ItemKind, InventoryItem);
}

void UInventoryManager::AddItemToInventory(FName ItemKind)
{
	if (Inventory)
	{
		Inventory->AddItemInstanceByName(ItemKind);
	}
}

void UInventoryManager::RemoveItemFromInventory(FName ItemKind)
{
	if (Inventory)
	{
		Inventory->RemoveItemInstanceByName(ItemKind);
	}
}

void UInventoryManager::RemoveItemsFromInventory(const TSet<FName>& ItemsToRemove)
{
	if (Inventory)
	{
		Inventory->RemoveItemKindsFromInventory(ItemsToRemove);
	}
}

bool UInventoryManager::IsInInventory(const FName& ItemToCheck) const
{
	return Inventory && Inventory->Contains(ItemToCheck);
}

UItem* UInventoryManager::GetItemFromInventory(const FName& ItemToCheck)
{
	if (Inventory)
	{
		return Inventory->FindItemByName(ItemToCheck);
	}
	return nullptr;
}

void UInventoryManager::GetInventoryItems(TArray<UItem*>& Items)
{
	if (Inventory)
	{
		Inventory->GetInventoryItemsArray(Items);
	}
}

int UInventoryManager::GetInventoryItemCount() const
{
	return Inventory ? Inventory->GetInventorySize() : 0;
}

void UInventoryManager::GetCustomInventoryItem(FName ItemKind)
{
	UE_LOG(LogAdventureGame, VeryVerbose, TEXT("UAdventureGameInstance::GetCustomInventoryItem: %s"), *ItemKind.ToString());
	const auto ItemBehavioursTable = ItemTableProvider->GetItemBehavioursTable();
	if (UDataTable *Table = ItemBehavioursTable.Get())
	{
		GetCustomInventoryItemWithTable(ItemKind, Table);
		return;
	}
	if (ItemBehavioursTable.IsPending())
	{
		TableOperationsQueue.Push(ItemKind);
		int32 Id = ItemBehavioursTable.LoadAsync(LoadTableDelegate);
		UE_LOG(LogAdventureGame, VeryVerbose, TEXT("  queueing: id %d - %d'th entry"), Id, TableOperationsQueue.Num());
		return;
	}
	/// There is no custom UInventoryItems at all, because there is no table. This is expected
	/// if no custom item behaviour is needed.
	UE_LOG(LogAdventureGame, Warning, TEXT("AdventureGameInstance::GetCustomInventoryItem: No ItemBehavioursTable set."));
	CustomInventoryItemLoadedDelegate.Broadcast(ItemKind, nullptr);
}

void UInventoryManager::RegenerateInventory(const TArray<FName> &InventoryKinds)
{
	DestroyInventory();
	CreateInventory();
	for (const FName Item : InventoryKinds)
	{
		Inventory->AddItemInstanceByName(Item);
	}
	BindInventoryChangedHandlers();
	OnInventoryChanged.Broadcast(NAME_None, EItemDisposition::Reloaded);
}

void UInventoryManager::NotifyInventoryChanged(TFunctionRef<void(FInventoryChangedDelegate &)> Subscriber)
{
	Subscriber(OnInventoryChanged);
}

void UInventoryManager::CustomInventoryItemLoaded(TFunctionRef<void(FCustomInventoryItemLoaded &)> Subscriber)
{
	Subscriber(CustomInventoryItemLoadedDelegate);
}

void UInventoryManager::CreateInventory()
{
	if (!Inventory)
	{
		Inventory = MakeShared<FInventory>();
		Inventory->SetupHandlers();
		UE_LOG(LogAdventureGame, Log, TEXT("Created new inventory."));
		Inventory->Identifier = PLAYER_INVENTORY_NAME;
	}
}

void UInventoryManager::DestroyInventory()
{
	Inventory->OnInventoryChanged.Remove(OnInventoryChangedHandle);
	Inventory->TearDownHandlers();
	Inventory = nullptr;
}

void UInventoryManager::BindInventoryChangedHandlers()
{
	if (!Inventory->OnInventoryChanged.IsBound())
	{
		OnInventoryChangedHandle = Inventory->OnInventoryChanged.AddUObject(this, &UInventoryManager::InventoryChanged);
	}
}
